
// I_SOUND.C

#include <stdio.h>
#include "h2def.h"
#include "sounds.h"

/*
===============
=
= I_StartupTimer
=
===============
*/

void I_StartupTimer (void)
{

}

void I_ShutdownTimer (void)
{

}

/*
 *
 *                           SOUND HEADER & DATA
 *
 *
 */

// sound information
#if 0
const char *dnames[] = {"None",
			"PC_Speaker",
			"Adlib",
			"Sound_Blaster",
			"ProAudio_Spectrum16",
			"Gravis_Ultrasound",
			"MPU",
			"AWE32"
			};
#endif

const char snd_prefixen[] = { 'P', 'P', 'A', 'S', 'S', 'S', 'M',
  'M', 'M', 'S' };

int snd_Channels;
int snd_DesiredMusicDevice, snd_DesiredSfxDevice;
int snd_MusicDevice,    // current music card # (index to dmxCodes)
	snd_SfxDevice,      // current sfx card # (index to dmxCodes)
	snd_MaxVolume,      // maximum volume for sound
	snd_MusicVolume;    // maximum volume for music

int     snd_SBport, snd_SBirq, snd_SBdma;       // sound blaster variables
int     snd_Mport;                              // midi variables

extern boolean  snd_MusicAvail, // whether music is available
		snd_SfxAvail;   // whether sfx are available

void I_PauseSong(int handle)
{
}

void S_StartSong(int song,boolean loop) {
}

void S_GetChannelInfo(SoundInfo_t *s)
{
	int i;
	ChanInfo_t *c;

	s->channelCount = 0;
	s->musicVolume = 0;
	s->soundVolume = 0;
/*	for(i = 0; i < snd_Channels; i++)
	{
		c = &s->chan[i];
		c->id = channel[i].sound_id;
		c->priority = channel[i].priority;
		c->name = S_sfx[c->id].name;
		c->mo = channel[i].mo;
		c->distance = P_AproxDistance(c->mo->x-viewx, c->mo->y-viewy)
			>>FRACBITS;
	}*/
}

void S_SetMaxVolume(boolean fullprocess) {

}

void S_StartSound(mobj_t *origin, int sound_id)
{
}

void S_StartSoundAtVolume(mobj_t *origin, int sound_id,int vol)
{
}

void S_SetMusicVolume(void) {
}

void S_Start(void) {
}

void S_StopSound(mobj_t *origin) {
}

void S_ResumeSound(void) {
}

void S_UpdateSounds(mobj_t *listener) {
}

void S_PauseSound(void) {
}

void S_StartSongName(char *songLump, boolean loop)
{
}



void I_ResumeSong(int handle)
{
}

void I_SetMusicVolume(int volume)
{
}

void I_SetSfxVolume(int volume)
{
}

/*
 *
 *                              SONG API
 *
 */

int I_RegisterSong(void *data)
{
  return -1;
}

void I_UnRegisterSong(int handle)
{
}

int I_QrySongPlaying(int handle)
{
  return -1;
}

// Stops a song.  MUST be called before I_UnregisterSong().

void I_StopSong(int handle)
{
}

void I_PlaySong(int handle, int loop)
{
}

/*
 *
 *                                 SOUND FX API
 *
 */

// Gets lump nums of the named sound.  Returns pointer which will be
// passed to I_StartSound() when you want to start an SFX.  Must be
// sure to pass this to UngetSoundEffect() so that they can be
// freed!


int I_GetSfxLumpNum(sfxinfo_t *sound)
{
  return W_GetNumForName(sound->lumpname);
}

int I_StartSound (int id, void *data, int vol, int sep, int pitch, int priority)
{
  return -1;
}

void I_StopSound(int handle)
{
}

int I_SoundIsPlaying(int handle)
{
 return 0;
}

void I_UpdateSoundParams(int handle, int vol, int sep, int pitch)
{
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
}

// shuts down all sound stuff

void I_ShutdownSound (void)
{
}

void I_SetChannels(int channels)
{
}

boolean S_GetSoundPlayingInfo(mobj_t *mobj, int sound_id) {
 return false;
}

int S_GetSoundID(char *name) {
 return 0;
}

void S_StopAllSound(void) {
}
