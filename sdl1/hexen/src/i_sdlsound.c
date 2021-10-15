#include <stdio.h>
#include <math.h>       // pow()
#include <SDL.h>
#include <SDL_mixer.h>
#include "qmus2mid.h"
#include "h2def.h"
#include "sounds.h"
#include "i_sound.h"

extern char *SavePath;
Mix_Music *CurrentSong;
int SongPlaying;
int audio_opened;


#define SAMPLE_TYPE     short


/*
 *
 *                           SOUND HEADER & DATA
 *
 *
 */

int tsm_ID = -1;

const char snd_prefixen[] = { 'P', 'P', 'A', 'S', 'S', 'S', 'M',
  'M', 'M', 'S' };

int snd_Channels;
int	snd_MaxVolume;      // maximum volume for sound
int	snd_MusicVolume;    // maximum volume for music


void Postmix(void *userdata, Uint8 *stream, int len);

void au_close(void)
{
 Mix_CloseAudio();
}

int au_open(void)
{
 int ret;

 ret=Mix_OpenAudio(11025,AUDIO_S16LSB,2,512);
 if (ret>=0) {
  printf("SDL Audio opened successfully.\n");
  Mix_SetPostMix(Postmix,NULL);
  SongPlaying=0;
  audio_opened=1;
  return 1; 
 }
 return 0;
}

void I_PauseSong(int handle)
{
 Mix_PauseMusic();
}

void I_ResumeSong(int handle)
{
  Mix_ResumeMusic();
}

void I_SetMusicVolume(int volume)
{
 Mix_VolumeMusic(volume*8);
}

void I_SetSfxVolume(int volume)
{
}

/*
 *
 *                              SONG API
 *
 */

int I_RegisterSong(void *data, int size)
{
  FILE *f;
  int sl;
#ifndef MUSTDIE
  char *fn="/CurrentSong.mid";
#else
  char *fn="\\CURSONG.MID";
#endif
  char *SongPath;

  if (SongPlaying) {
   I_StopSong(0);
   SDL_Delay(20);
   I_UnRegisterSong(0);
  }

  sl=strlen(SavePath);
  SongPath=malloc(sl+strlen(fn)+1);
  strcpy(SongPath,SavePath);
  strcat(SongPath,fn);
  f=fopen(SongPath,"wb");
  if (!audio_opened) 
   au_open();
  if (f) {
   qmus2mid(data,size,f,1,0,0,0);
   fclose(f);
   CurrentSong=Mix_LoadMUS(SongPath);
   if (!CurrentSong) {
     printf("MIDI load failed:%s\n",Mix_GetError());
   }
  }
  free(SongPath);
  return 0;
}

void I_UnRegisterSong(int handle)
{
  if (CurrentSong) {
   Mix_FreeMusic(CurrentSong);
   CurrentSong=NULL;
  }
}

int I_QrySongPlaying(int handle)
{
 return SongPlaying; /* XXX */
}

// Stops a song.  MUST be called before I_UnregisterSong().

void I_StopSong(int handle)
{
  Mix_FadeOutMusic(500);
  SongPlaying=0;
}

void I_PlaySong(int handle, boolean looping)
{
 if (SongPlaying) return;
 Mix_FadeInMusic(CurrentSong,looping?-1:0,500);
 SongPlaying=1;
}


/*
 *
 *                                 SOUND FX API
 *
 */


typedef struct
{
    unsigned char* begin;           // pointers into Sample.firstSample
    unsigned char* end;

    SAMPLE_TYPE* lvol_table;               // point into vol_lookup
    SAMPLE_TYPE* rvol_table;

    unsigned int pitch_step;
    unsigned int step_remainder;    // 0.16 bit remainder of last step.
    
    int pri;
    unsigned int time;
} Channel;


typedef struct
{
    short a;            // always 3
    short freq;         // always 11025
    long length;        // sample length
    unsigned char firstSample;
} Sample;


#define CHAN_COUNT        8
Channel schannel[ CHAN_COUNT];

#define MAX_VOL           64        // 64 keeps our table down to 16Kb
SAMPLE_TYPE vol_lookup[ MAX_VOL * 256 ];

int steptable[ 256 ];               // Pitch to stepping lookup


void Postmix(void *userdata, Uint8 *stream, int len)
{
    Channel* chan;
    Channel* cend;
    SAMPLE_TYPE* begin;
    SAMPLE_TYPE* end;
    unsigned int sample;
    register int dl;
    register int dr;
    
    end = (SAMPLE_TYPE*) (stream + len);
    cend = schannel + CHAN_COUNT;
    
    begin = (SAMPLE_TYPE*) stream;
    while( begin < end )
        {
            // Mix all the channels together.

            dl = 0;  /* zero sample */
            dr = 0;
            chan = schannel;
            for( ; chan < cend; chan++ )
            {
                // Check channel, if active.
                if( chan->begin )
                {
                    // Get the sample from the channel. 
                    sample = *chan->begin;

                    // Adjust volume accordingly.
                    dl += chan->lvol_table[ sample ];
                    dr += chan->rvol_table[ sample ];

                    // Increment sample pointer with pitch adjustment.
                    chan->step_remainder += chan->pitch_step;
                    chan->begin += chan->step_remainder >> 16;
                    chan->step_remainder &= 65535;

                    // Check whether we are done.
                    if( chan->begin >= chan->end )
                    {
                        chan->begin = 0;
                    }
                }
            }
            
            if( dl > 0x7fff ) dl = 0x7fff;
            else if( dl < -0x8000 ) dl = -0x8000;

            if( dr > 0x7fff ) dr = 0x7fff;
            else if( dr < -0x8000 ) dr = -0x8000;
	    
	    sample=*begin + dl;		
            *begin++ = sample/2;
	    sample=*begin + dl;		
            *begin++ = sample/2;
        }
  
}


// Gets lump nums of the named sound.  Returns pointer which will be
// passed to I_StartSound() when you want to start an SFX.  Must be
// sure to pass this to UngetSoundEffect() so that they can be
// freed!


int I_GetSfxLumpNum(sfxinfo_t *sound)
{
  return W_GetNumForName(sound->lumpname);
}


// Id is unused.
// Data is a pointer to a Sample structure.
// Volume ranges from 0 to 127.
// Separation (orientation/stereo) ranges from 0 to 255.  128 is balanced.
// Pitch ranges from 0 to 255.  Normal is 128.
// Priority looks to be unused (always 0).

int I_StartSound( int id, void* data, int vol, int sep, int pitch, int priority)
{
    // Relative time order to find oldest sound.
    static unsigned int soundTime = 0;
    int chanId;
    Sample* sample;
    Channel* chan;
    int oldest;
    int i;
    // Find an empty channel, the oldest playing channel, or default to 0.
    // Currently ignoring priority.

    chanId = 0;
    oldest = soundTime;
    for( i = 0; i < CHAN_COUNT; i++ )
    {
        if( ! schannel[ i ].begin )
        {
            chanId = i;
            break;
        }
        if( schannel[ i ].time < oldest )
        {
            chanId = i;
            oldest = schannel[ i ].time;
        }
    }

    sample = (Sample*) data;
    chan = &schannel[ chanId ];

    I_UpdateSoundParams( chanId + 1, vol, sep, pitch );

    chan->pri = priority;
    chan->time = soundTime;
    chan->end = &sample->firstSample + sample->length;
    chan->begin = &sample->firstSample;

    soundTime++;

    return chanId + 1;
}

void I_StopSound(int handle)
{
    handle--;
    handle &= 7;
    schannel[ handle ].begin = 0;
}

int I_SoundIsPlaying(int handle)
{
    handle--;
    handle &= 7;
    return( schannel[ handle ].begin != 0 );
}

void I_UpdateSoundParams(int handle, int vol, int sep, int pitch)
{
    int lvol, rvol;
    Channel* chan;

    // Set left/right channel volume based on seperation.

    sep += 1;       // range 1 - 256
    lvol = vol - ((vol * sep * sep) >> 16); // (256*256);
    sep = sep - 257;
    rvol = vol - ((vol * sep * sep) >> 16);    


    // Sanity check, clamp volume.
    if( rvol < 0 )
    {
        //printf( "rvol out of bounds %d, id %d\n", rvol, handle );
        rvol = 0;
    }
    else if( rvol > 127 )
    {
        //printf( "rvol out of bounds %d, id %d\n", rvol, handle );
        rvol = 127;
    }
    
    if( lvol < 0 )
    {
        //printf( "lvol out of bounds %d, id %d\n", lvol, handle );
        lvol = 0;
    }
    else if( lvol > 127 )
    {
        //printf( "lvol out of bounds %d, id %d\n", lvol, handle );
        lvol = 127;
    }

    // Limit to MAX_VOL (64)
    lvol >>= 1;
    rvol >>= 1;

    handle--;
    handle &= 7;
    chan = &schannel[ handle ];
    chan->pitch_step = steptable[ pitch ];
    chan->step_remainder = 0;
    chan->lvol_table = &vol_lookup[ lvol * 256 ];
    chan->rvol_table = &vol_lookup[ rvol * 256 ];
}

/*
 *
 *                                                      SOUND STARTUP STUFF
 *
 *
 */

// inits all sound stuff

void I_StartupSound (void)
{
    int ok;

//    snd_MusicDevice = snd_SfxDevice = 0;

    if( M_CheckParm( "-nosound" ) )
    {
        ST_Message("I_StartupSound: Sound Disabled.\n");
        return;
    }

    if (debugmode)
        ST_Message("I_StartupSound: Hope you hear a pop.\n");

    if (!audio_opened)
     ok = au_open();
    
}

// shuts down all sound stuff

void I_ShutdownSound (void)
{
	I_StopSong(0);
	I_UnRegisterSong(0);
        au_close();
}

void I_SetChannels(int channels)
{
    int v, j;
    int* steptablemid;

    // We always have CHAN_COUNT channels.

    for( j = 0; j < CHAN_COUNT; j++ )
    {
        schannel[ j ].begin = 0;
        schannel[ j ].end = 0;
        schannel[ j ].time = 0;
    }


    // This table provides step widths for pitch parameters.
    steptablemid = steptable + 128;
    for( j = -128; j < 128; j++ )
    {
        steptablemid[ j ] = (int) (pow( 2.0, (j/64.0) ) * 65536.0);
    }


    // Generate the volume lookup tables.
    for( v = 0 ; v < MAX_VOL ; v++ )
    {
        for( j = 0; j < 256; j++ )
        {
            //vol_lookup[v*256+j] = 128 + ((v * (j-128)) / (MAX_VOL-1));

            // Turn the unsigned samples into signed samples.
            vol_lookup[v*256+j] = (v * (j-128) * 256) / (MAX_VOL-1);

            //printf("vol_lookup[%d*256+%d] = %d\n", v, j, vol_lookup[v*256+j]);
        }
    }
}


/* EOF */
