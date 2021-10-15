#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <SDL_audio.h>
#include <SDL_error.h>

char dirlist[]="data";

int readsound(int num);

#define NUMSOUNDS	(sizeof(soundnames)/sizeof(char*))
#define MIXMAX 16

#define SOUND_QUIET -1

char *soundnames[] =
{
"bomb1.raw",
"power1.raw",
"death.raw",
"drop.raw",
"bomb2.raw",
"power2.raw",
};
typedef struct sample
{
	char *data;
	int len;
} sample;

#define SNDFRAGMENT 1024

sample samples[NUMSOUNDS];

int soundworking=0;
int fragment;
int soundwrite,soundread;
int *soundbuffer;
int soundbufferlen;
unsigned char sndclip[8192];

#define MAXSOUNDCOMMANDS 32
char soundcommands[MAXSOUNDCOMMANDS];
int soundtake,soundput;

int sndplaying[MIXMAX],sndposition[MIXMAX];
void fillaudio(void *udata,Uint8 *buffer,int len)
{
char com,*p;
int i,j,*ip;
int which;

	while(soundtake!=soundput)
	{
		com=soundcommands[soundtake];
		soundtake=(soundtake+1)&(MAXSOUNDCOMMANDS-1);
		if(com==SOUND_QUIET) {memset(sndposition,0,sizeof(sndposition));continue;}
		if(com<NUMSOUNDS)
		{
			for(i=0;i<MIXMAX;++i)
				if(!sndposition[i])
				{
					sndposition[i]=1;
					sndplaying[i]=com;
					break;
				}
		}
	}
	memset(soundbuffer,0,soundbufferlen);
	for(i=0;i<MIXMAX;++i)
	{
		if(!sndposition[i]) continue;
		which=sndplaying[i];
		if(sndposition[i]==samples[which].len)
		{
			sndposition[i]=0;
			continue;
		}
		p=samples[which].data;
		if(!p) continue;
		p+=len*(sndposition[i]++ -1);
		ip=soundbuffer;
		j=len;
		while(j--) *ip++ += *p++;
	}
	j=len;
	ip=soundbuffer;;
	while(j--) *buffer++ = sndclip[4096+*ip++];

}


int soundopen(void)
{
SDL_AudioSpec wanted;
int i,j;

	soundtake=soundput=0;
	memset(sndposition,0,sizeof(sndposition));
	memset(sndplaying,0,sizeof(sndplaying));
	fragment=SNDFRAGMENT<<1;
	soundbufferlen=fragment*sizeof(int);
	soundbuffer=malloc(soundbufferlen);
	if(!soundbuffer) return -2;

	memset(&wanted,0,sizeof(wanted));
	wanted.freq=22050;
	wanted.channels=2;
	wanted.format=AUDIO_U8;
	wanted.samples=fragment>>1;
	wanted.callback=fillaudio;
	wanted.userdata=0;

	if(SDL_OpenAudio(&wanted,0)<0)
	{
		fprintf(stderr,"Couldn't open audio: %s\n",SDL_GetError());
		return -1;
	}
	soundworking=1;

	for(i=0;i<8192;i++)
	{
		j=i-4096;
		sndclip[i]=j > 127 ? 255 : (j<-128 ? 0 : j+128);
	}

	for(i=0;i<NUMSOUNDS;++i)
		readsound(i);

	SDL_PauseAudio(0);
	return 0;
}
void soundclose(void)
{
	if(soundworking)
	{
		SDL_CloseAudio();
		soundworking=0;
	}
}


int readsound(int num)
{
char name[256],*p1,*p2,ch;
int i,file,size,len;
	p1=dirlist;
	for(;;)
	{
		p2=name;
		while(*p1 && (ch=*p1++)!=',')
			*p2++=ch;
		if(p2>name && p2[-1]!='/') *p2++='/';
		strcpy(p2,soundnames[num]);
		file=_open(name,O_RDONLY);
		if(file>=0) break;
		if(!*p1)
		{
			samples[num].len=-1;
			return 0;
		}
	}
	size=lseek(file,0,SEEK_END);
	lseek(file,0,SEEK_SET);
	len=samples[num].len=(size+fragment-1)/fragment;
	len*=fragment;
	p1=samples[num].data=malloc(len);
	if(p1)
	{
		i=_read(file,p1,size);
		if(len-size) memset(p1+size,0,len-size);
		while(size--) *p1++ ^= 0x80;
	} else
		samples[num].data=0;
	_close(file);
	return 0;
}

void playsound(int n)
{
	soundcommands[soundput]=n;
	soundput=(soundput+1)&(MAXSOUNDCOMMANDS-1);
}
