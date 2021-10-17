// Xargon SDL port
// Copyright 2011 Adam Nielsen <malvineous@shikadi.net>

#include <algorithm>
#include "port.h"
#include "dbopl.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <SDL_mutex.h>
#include <synchapi.h>

#ifdef USE_MUSIC
#include <camoto/gamemusic.hpp>
#include <camoto/stream_file.hpp>
#include <camoto/stream_string.hpp>
#include <camoto/gamemusic/manager.hpp>
namespace gm = camoto::gamemusic;
#endif

SDL_Surface *screen;
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

#include <stdio.h>


#ifndef __WIN32
char *itoa(int val, char *str, int base)
{
	sprintf(str, "%d", val);
	return str;
}
char *ltoa(long val, char *str, int base)
{
	sprintf(str, "%ld", val);
	return str;
}
char *strupr(char *s)
{
	char *o = s;
	do {
		if ((*s >= 'a') && (*s <= 'z')) *s -= 'a' - 'A';
	} while (*s++);
	return o;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
unsigned long filelength(int fd)
{
	struct stat buf;
	fstat(fd, &buf);
	return buf.st_size;
}
#endif

unsigned long coreleft(void)
{
	return 1234;
}

void delay(int ms)
{
	//usleep(ms * 1000);
	Sleep(ms);
	return;
}

int getclock()
{
	return SDL_GetTicks() / 54.9450549;
}

void cputs(const char *s)
{
	printf("%s", s);
	fflush(stdout);
	return;
}

void textcolor(int c)
{
//	printf("\033[%d;%dm", (c & 8) ? 1 : 22, 30 + (((c & 1) << 2) | ((c & 4) >> 2) | (c & 2)));
	return;
}

void textbackground(int c)
{
//	printf("\033[%dm", 40 + ((c & 1) << 2) | ((c & 4) >> 2) | (c & 2));
	return;
}

void enable(void)
{
	// ???
}

void disable(void)
{
	// ???
}

void window(int x, int y, int w, int h)
{
	// ???
}

void gotoxy(int x, int y)
{
//	printf("\033[%d;%dH", x, y);
	return;
}

void clrscr(void)
{
//	printf("\033[2J");
	return;
}

#include <stdlib.h>
#include <cstdlib>
int xr_random(int max)
{
	return rand() / (RAND_MAX / max);
}

#include <string.h>
void setmem(void *target, unsigned long len, char val)
{
	memset(target, val, len);
	return;
}

unsigned char inportb(unsigned int port)
{
	switch (port) {
		case 0x201: // joystick
			return 0;
	}
	fprintf(stderr, "Read from unknown portb 0x%02X\n", port);
	return 0;
}

unsigned char outportb(unsigned int port, unsigned char value)
{
	switch (port) {
		case 0x201: // joystick
			break;
		case 0x3C8: // VGA
		case 0x3C9:
			break;
		default:
			fprintf(stderr, "Write to unknown portb 0x%04X: 0x%02X\n", port, value);
			break;
	}
	return 0;
}

unsigned char outport(unsigned int port, unsigned int value)
{
	switch (port) {
		case 0x201: // joystick
			return 0;
		case 0x3C4: // VGA
		case 0x3CE:
		case 0x3D4:
			break;
		default:
			fprintf(stderr, "Write to unknown portw 0x%04X: 0x%04X\n", port, value);
			break;
	}
	return 0;
}

void intr(int intnum, struct REGPACK *regs)
{
	switch (intnum) {
		case 0x10: // video
			printf("TODO: video int 10 ax %04X\n", regs->r_ax);
			break;
	}
	return;
}

void nosound(void)
{
	return;
}

void spkr_intr(void)
{
}

void setvect(int intnum, void (*handler)(void))
{
	// TODO
	return;
}

intfunc getvect(int interrupt)
{
	return NULL;
}

int k_pressed(void)
{
	SDL_Event ev;
	SDL_PumpEvents();
	return SDL_PeepEvents(&ev, 1, SDL_PEEKEVENT, SDL_KEYDOWNMASK);
}

int k_read(void)
{
	SDL_Event ev;
	while (SDL_WaitEvent(&ev)) {
		if (ev.type == SDL_KEYDOWN) {
			return ev.key.keysym.sym;
		}
	}
	return -1;
}

char k_rshift, k_lshift, k_shift, k_ctrl, k_alt, k_numlock;

void k_status(void)
{
	SDLMod state = SDL_GetModState();
	k_rshift = (state & KMOD_RSHIFT) ? 1 : 0;
	k_lshift = (state & KMOD_LSHIFT) ? 1 : 0;
	k_shift = (state & KMOD_SHIFT) ? 1 : 0;
	k_ctrl = (state & KMOD_CTRL) ? 1 : 0;
	k_alt = (state & KMOD_ALT) ? 1 : 0;
	k_numlock = (state & KMOD_NUM) ? 1 : 0;
	return;
}

unsigned char keydown[2][256];

void installhandler(unsigned char status)
{
	// no op
	return;
}

void removehandler(void)
{
	// no op
	return;
}

#include "include/gr.h"
void clrvp (vptype *vp,byte col)
{
	if (SDL_MUSTLOCK(::screen)) {
		if (SDL_LockSurface(::screen) < 0) return;
	}

	// TODO: optimise by incrementing shape ptr instead
	for (int y = 0; y < vp->vpyl; y++) {
		for (int x = 0; x < vp->vpxl; x++) {
			((char *)::screen->pixels)[(vp->vpy + y) * SCREEN_WIDTH + (vp->vpx + x)] = col;
		}
	}

	if (SDL_MUSTLOCK(::screen)) SDL_UnlockSurface(::screen);
	return;
}

void scrollvp (vptype *vp,int xd,int yd)
{
	// TODO: This function is required to scroll message boxes, but it doesn't
	// seem to be needed for in-game scrolling, and seems to slow things down.
	printf("todo: scrollvp(%d,%d)\n", xd, yd);

	if (SDL_MUSTLOCK(::screen)) {
		if (SDL_LockSurface(::screen) < 0) return;
	}

	int ydir = 0, ystart = 0, yend = 0;
	if (yd < 0) {
		ydir = 1;
		ystart = vp->vpy;
		yend = ystart + vp->vpyl;
	} else {
		ydir = -1;
		yend = vp->vpy - 1;
		ystart = yend + vp->vpyl;
	}
	int xsrc, xdest, xlen;
	if (xd < 0) {
		xsrc = vp->vpx - xd;
		xdest = vp->vpx;
		xlen = vp->vpxl + xd;
	} else {
		xsrc = vp->vpx;
		xdest = vp->vpx + xd;
		xlen = vp->vpxl - xd;
	}
	uint8_t *s = (uint8_t *)::screen->pixels;
	for (int y = ystart; (yd < 0) ? (y < yend) : (y > yend && y >= yd ); y += ydir) {
		memcpy(
			&s[y * SCREEN_WIDTH + xdest],
			&s[(y - yd) * SCREEN_WIDTH + xsrc],
			xlen
		);
	}

	if (SDL_MUSTLOCK(::screen)) SDL_UnlockSurface(::screen);

	if (drawofs == showofs) 
		SDL_Flip(::screen);
	return;
}

void scroll (vptype *vp,int x0,int y0,int x1,int y1,int xd,int yd)
{
	// TODO
	printf("todo: scroll\n");
	return;
}

void plot_vga (int x, int y, byte color)
{
	if (SDL_MUSTLOCK(::screen)) {
		if (SDL_LockSurface(::screen) < 0) return;
	}

	((char *)::screen->pixels)[y * SCREEN_WIDTH + x] = color;

	if (SDL_MUSTLOCK(::screen)) SDL_UnlockSurface(::screen);

	if (drawofs == showofs) SDL_Flip(::screen);
	return;
}

#include <fcntl.h>
void ldrawsh_vga (vptype *vp, int draw_x, int draw_y, int sh_xlb, int sh_yl,
	char far *shape, int cmtable)
{
	// Crop the draw to fit within the viewport
	int startx = 0, y = 0;
	if (draw_y < 0) y = -draw_y;
	if (draw_x < 0) startx = -draw_x;

	int actual_width;
	if (draw_x + sh_xlb > vp->vpxl) {
		actual_width = vp->vpxl - draw_x;
	} else {
		actual_width = sh_xlb;
	}
	if (draw_y + sh_yl > vp->vpyl) {
		sh_yl = vp->vpyl - draw_y;
	}

	// Convert the viewport coords into screen coords
	draw_x += vp->vpx;
	draw_y += vp->vpy;
	if ((draw_x+startx < 0) || (draw_y+y < 0)) {
		printf("Tried to draw at negative screen coordinates! (%d,%d)\n", draw_x, draw_y);
		return;
	}

	if (SDL_MUSTLOCK(::screen)) {
		if (SDL_LockSurface(::screen) < 0) return;
	}

	// TODO: optimise by incrementing shape ptr instead
	for (; y < sh_yl; y++) {
		for (int x = startx; x < actual_width; x++) {
			uint8_t pixel = shape[y * sh_xlb + x];
			pixel = cmtab[cmtable][pixel];
			if (pixel == 255) continue; // transparent
			((char *)::screen->pixels)[(draw_y + y) * SCREEN_WIDTH + (draw_x + x)] = pixel;
		}
	}

	if (SDL_MUSTLOCK(::screen)) SDL_UnlockSurface(::screen);

	if (drawofs == showofs) SDL_Flip(::screen);
	return;
}

void lcopypage(void)
{
	SDL_Flip(::screen);
	return;
}

void uncrunch (char *sourceptr, char *destptr, int length)
{
	// TODO
	if (destptr == (char *)0xb8000000) {
		printf("decrunch to text screen\n");
	} else {
		memset(destptr, 0, length);
	}
	return;
}

void WorxBugInt8(void)
{
}

// Clipping function to prevent integer wraparound after amplification
#define SAMPLE_SIZE 2
#define SAMP_BITS (SAMPLE_SIZE << 3)
#define SAMP_MAX ((1 << (SAMP_BITS-1)) - 1)
#define SAMP_MIN -((1 << (SAMP_BITS-1)))
#define CLIP(v) (((v) > SAMP_MAX) ? SAMP_MAX : (((v) < SAMP_MIN) ? SAMP_MIN : (v)))

class SampleHandler: public MixerChannel {
	public:
		uint8_t *buf;

		virtual void AddSamples_m32(Bitu samples, Bit32s *buffer)
		{
			// Convert samples from mono s32 to s16
			int16_t *out = (int16_t *)this->buf;
			for (int i = 0; i < samples; i++) {
				*out++ = CLIP(buffer[i]);
				*out++ = CLIP(buffer[i]);
			}
			return;
		}

		virtual void AddSamples_s32(Bitu samples, Bit32s *buffer)
		{
			// Convert samples from stereo s32 to s16
			int16_t *out = (int16_t *)this->buf;
			for (int i = 0; i < samples * 2; i++) {
				*out++ = CLIP(buffer[i]);
			}
			return;
		}
};

extern "C" {

struct oplinfo {
	SampleHandler *sh;
	DBOPL::Handler *chip;
	uint8_t *cur_song;     // pointer to start of song file
	int song_pos;          // current file pointer in cur_song
	int song_len;          // length of song in bytes
	int delay_remaining;   // delay until next music event in number of audio buffers
	SDL_mutex *mutex;
	int chipIndex;
} opl;

struct sounddata {
	uint8_t *data;
	int len;
	int samplerate;
	int vol;
	int pos;
} sound;

/// Mix two PCM samples and return the new combined sample
inline long pcm_mix_s16(long a, long b)
{
	a += 32768;
	b += 32768;
	unsigned long m;
	if ((a < 32768) && (b < 32768)) {
		m = (long long)a * b / 32768LL;
	} else {
		m = 2 * ((long long)a + b) - ((long long)a * b) / 32768LL - 65536;
	}
	if (m == 65536) m = 65535;
	return -32768 + m;
}

//#define min(a, b) (((a) < (b)) ? (a) : (b))
int16_t sound_buffer[4096];//2048];
void fillAudioBuffer(void *udata, Uint8 *stream, int len)
{
	int bufvalid_bytes = min(len, sizeof(sound_buffer));
	int bufvalid_samples = bufvalid_bytes / sizeof(int16_t) / 2;

	// Mix in the music
	SDL_mutexP(::opl.mutex);
	if (::opl.cur_song) {
		::opl.sh->buf = (uint8_t *)sound_buffer;
		int opl_samples = bufvalid_samples;
		do {
#ifdef USE_MUSIC
			while (::opl.delay_remaining == 0) {
				// No more delay, process song data until next delay
				uint8_t reg = ::opl.cur_song[::opl.song_pos];
				uint16_t nextDelay = 0;
				switch (reg) {
					case 0x00:
						nextDelay = ::opl.cur_song[::opl.song_pos + 1];
						::opl.song_pos += 2;
						break;
					case 0x01:
						nextDelay = ::opl.cur_song[::opl.song_pos + 1] | (::opl.cur_song[::opl.song_pos + 2] << 8);
						::opl.song_pos += 3;
						break;
					case 0x02:
						::opl.chipIndex = 0;
						::opl.song_pos++;
						break;
					case 0x03:
						::opl.chipIndex = 0x100;
						::opl.song_pos++;
						break;
					case 0x04:
						::opl.song_pos++;
						reg = ::opl.cur_song[::opl.song_pos];
						// fall through
					default:
						::opl.chip->WriteReg(::opl.chipIndex + reg, ::opl.cur_song[::opl.song_pos + 1]);
						::opl.song_pos += 2;
						break;
				}
				::opl.delay_remaining = nextDelay * (48000/512) / 1000;
				if (::opl.song_pos >= ::opl.song_len) ::opl.song_pos = 0; // loop
			}
#endif
			::opl.chip->Generate(::opl.sh, min(512, bufvalid_samples));
			opl_samples -= 512;
			::opl.sh->buf += 512 * sizeof(int16_t) * 2;
			::opl.delay_remaining--;
		} while (opl_samples >= 512);
	} else {
		// No music
		memset(sound_buffer, 0, bufvalid_bytes);
	}
	SDL_mutexV(::opl.mutex);

	// Play the next bit of the sound effect if needed
	if (::sound.len > 0) {
		// Take U8 sound at any rate, convert to S16 48kHz and mix with music
		double ratio = sound.samplerate / 48000.0;
		for (int i = 0; i < bufvalid_samples; i++) {
			int j = ::sound.pos + (int)(i * ratio);
			if (j >= ::sound.len) break;
			int16_t b = CLIP((::sound.data[j] - 127) * (254/2));
			sound_buffer[i*2] = pcm_mix_s16(sound_buffer[i*2], b);
			sound_buffer[i*2+1] = pcm_mix_s16(sound_buffer[i*2+1], b);
		}
		::sound.pos += bufvalid_samples * ratio; // TODO: might drop a sample or two...
		if (::sound.pos >= ::sound.len) ::sound.len = 0; // end of sound
	}
	SDL_MixAudio(stream, (Uint8 *)sound_buffer, bufvalid_bytes, SDL_MIX_MAXVOLUME);

	return;
}

// Cache a loaded music file so we don't have to reconvert it on each song change
typedef struct {
	char name[13];
	uint8_t *data;
} music_file;

std::vector<music_file> music_data;

void StartWorx(void)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	::screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 8, SDL_HWPALETTE | SDL_DOUBLEBUF);

	::opl.cur_song = NULL;
	::sound.len = 0;

	::opl.mutex = SDL_CreateMutex();

	SDL_AudioSpec wanted;
	wanted.freq = 48000;
	wanted.format = AUDIO_S16;
	wanted.channels = 2;
	wanted.samples = 2048;
	wanted.callback = fillAudioBuffer;
	wanted.userdata = NULL;
	if (SDL_OpenAudio(&wanted, NULL) < 0) {
		printf("Couldn't open sound device in given audio format - no sound!\n");
		//::vocflag = 0;
		//::musicflag = 0;
	}

	::opl.sh = new SampleHandler();
	::opl.chip = new DBOPL::Handler();
	::opl.chip->Init(48000);
	SDL_PauseAudio(0);

	return;
}

void CloseWorx(void)
{
	SDL_DestroyMutex(::opl.mutex);

	// Release any CMF files that were loaded
	for (std::vector<music_file>::iterator i = ::music_data.begin(); i != ::music_data.end(); i++) {
		delete[] i->data;
	}
	SDL_Quit();

	delete ::opl.chip;
	delete ::opl.sh;
}

int AdlibDetect(void)
{
	return 1;
}

int SetFMVolume(unsigned char left, unsigned char right)
{
	return 0;
}

int DSPReset(void)
{
	return 1; // DSP present
}

void DSPClose(void)
{
	SDL_CloseAudio();
	return;
}

int SetMasterVolume(unsigned char left, unsigned char right)
{
	return 0;
}

void StopSequence(void)
{
	SDL_mutexP(::opl.mutex);

	::opl.cur_song = NULL;
	// Silence the OPL chip
	::opl.chip->WriteReg(0xBD, 0x00);
	for (int i = 0; i < 9; i++) ::opl.chip->WriteReg(0xB0 + i, 0x00);

	SDL_mutexV(::opl.mutex);
	return;
}

char *GetSequence(char *f_name)
{
//#ifdef 1
	for (std::vector<music_file>::iterator i = ::music_data.begin(); i != ::music_data.end(); i++) {
		// If the file has already been loaded, return the existing data
		if (strcmp(f_name, i->name) == 0) return (char *)i->data;
	}

	// File hasn't been loaded, do that now
	camoto::gamemusic::ManagerPtr pManager(gm::getManager());
	camoto::stream::input_file_sptr psMusic(new camoto::stream::input_file());
	try {
		psMusic->open(f_name);
	} catch (const camoto::stream::open_error& e) {
		fprintf(stderr, "Error opening %s\n", f_name);
		return NULL;
	}
	gm::MusicTypePtr pMusicType(pManager->getMusicTypeByCode("cmf-creativelabs"));
	if (!pMusicType->isInstance(psMusic)) {
		printf("File %s is not in CMF format!\n", f_name);
		return NULL;
	}
	camoto::SuppData suppData;
	gm::MusicPtr pMusic(pMusicType->read(psMusic, suppData));
	assert(pMusic);

	camoto::stream::string_sptr pss(new camoto::stream::string());
	gm::MusicTypePtr pMusicOutType(pManager->getMusicTypeByCode("dro-dosbox-v1"));
	pMusicOutType->write(pss, suppData, pMusic, gm::MusicType::Default);

	music_file cmf;
	strcpy(cmf.name, f_name);
	std::string imfdata = *pss->str();
	int len = imfdata.length();
	cmf.data = new uint8_t[len];
	memcpy(cmf.data, imfdata.data(), len);
	::music_data.push_back(cmf);
	printf("Loaded CMF file %s\n", cmf.name);
	return (char *)cmf.data;
//#else
	//return NULL;
//#endif
}

void SetLoopMode(int m)
{
	// Always called with m=1 to loop music
	return;
}

int VOCPlaying(void)
{
	return ::sound.len > 0;
}

#define SND_CROP 2 // cut this many bytes off the start of the sound
int PlayVOCBlock(char *voc, int volume)
{
	uint8_t *vocdata = (uint8_t *)voc;
	if (strncmp(voc, "Creative Voice File\x1A", 20) != 0) {
		printf("Tried to play non-VOC file\n");
		return -1;
	}

	::sound.data = vocdata + 0x20 + SND_CROP; // TODO: use correct offset (only if nonstandard files are used?)
	switch (vocdata[0x1A]) {
		case 1: // normal sound
			if (vocdata[0x1F] != 0) {
				printf("Packed VOC data not implemented, ignoring sound\n");
				break;
			}
			::sound.len = (vocdata[0x1B] | (vocdata[0x1C] << 8) | (vocdata[0x1D] << 16)) - SND_CROP;
			::sound.vol = volume;
			::sound.pos = 0;
			::sound.samplerate = 1000000 / (256 - vocdata[0x1E]);
			//printf("Sample rate is %dHz (byte val %u)\n", ::sound.samplerate, vocdata[0x1E]);
			break;
		default:
			printf("VOC sound has unknown block type %d, ignoring\n", vocdata[0x1A]);
			break;
	}
	return 0;
}

void PlayCMFBlock(char *seq)
{
	SDL_mutexP(::opl.mutex);

	// Init the OPL chip
	::opl.chip->WriteReg(0x01, 0x20);
	::opl.chip->WriteReg(0xBD, 0x00);
	for (int i = 0; i < 9; i++) ::opl.chip->WriteReg(0xB0 + i, 0x00);

	uint8_t *s = (uint8_t *)seq;
	::opl.cur_song = s + 24; // 24 is to skip DROv1 header
	::opl.song_pos = 0;
	::opl.song_len = (s[16] | (s[17] << 8) | (s[18] << 16) | (s[19] << 24));
	::opl.delay_remaining = 0;
	::opl.chipIndex = 0;

	SDL_mutexV(::opl.mutex);
	return;
}

} // extern C
