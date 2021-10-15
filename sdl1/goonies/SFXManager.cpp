#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif


#ifdef _WIN32
#include "windows.h"
#else
#include "dirent.h"
#endif

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_mixer.h"

#include "stdlib.h"
#include "string.h"
#include "List.h"

#include "Symbol.h"
#include "sound.h"

#include "SoundManager.h"
#include "SFXManager.h"

// #include "debug.h"


void SFXManager::channel_finished(int channel)
{
} /* SFXManager::channel_finished */




SFXManager::SFXManager()
{
} /* SFXManager::SFXManager */


SFXManager::~SFXManager()
{
	ContinuousSFX *sfx;

    m_already_played.ExtractAll();
    delete []m_hash;
    m_hash = 0;

	while(!m_continuous_being_played.EmptyP()) {
		sfx = m_continuous_being_played.ExtractIni();
		Mix_HaltChannel(sfx->m_channel);
		delete sfx;
	} // whiel
} /* SFXManager::~SFXManager */



void SFXManager::next_cycle(void)
{
	// Clear SFX already played:
    m_already_played.ExtractAll();
} /* SFXManager::next_cycle */


void SFXManager::object_destroyed_notification(void *object)
{
	ContinuousSFX *sfx;
	List<ContinuousSFX> to_delete;

	m_continuous_being_played.Rewind();
	while(m_continuous_being_played.Iterate(sfx)) {
		if (sfx->m_object==object) to_delete.Add(sfx);
	} // while

	while(!to_delete.EmptyP()) {
		sfx = to_delete.ExtractIni();
		SFX_stop(sfx->m_channel);
	} // while
}

int SFXManager::SFX_play(char *sfx, int volume)
{
	Mix_Chunk *s = get(sfx);
	
	if (m_already_played.MemberRefP(s)) return -1;

	if (s!=0) {
		int channel = Mix_PlayChannel( -1, s, 0);
		Mix_Volume(channel, volume);
		m_already_played.Add(s);

#ifdef __DEBUG_MESSAGES
	    output_debug_message("SFXManager::SFX_play(%s), playing SFX in channel %i\n",sfx,channel);
#endif
		return channel;
	} // if
	return -1;
} /* SFXManager::SFX_play */


int SFXManager::SFX_play(char *sfx, int volume, int angle, int distance)
{
	Mix_Chunk *s = get(sfx);

	if (m_already_played.MemberRefP(s)) return -1;

	if (s!=0) {
		int channel = Mix_PlayChannel( -1, s, 0);
		Mix_Volume(channel, volume);
		Mix_SetPosition(channel, angle, distance);
		m_already_played.Add(s);

#ifdef __DEBUG_MESSAGES
	    output_debug_message("SFXManager::SFX_play(%s), playing SFX in channel %i\n",sfx,channel);
#endif

		return channel;
	} // if
	return -1;
} /* SFXManager::SFX_play */


int SFXManager::SFX_play_channel(char *sfx, int channel, int volume)
{
	Mix_Chunk *s = get(sfx);

	if (s!=0) {
		Mix_PlayChannel(channel, s, 0);
		Mix_Volume(channel, volume);

#ifdef __DEBUG_MESSAGES
	    output_debug_message("SFXManager::SFX_play_channel(%s), playing SFX in channel %i\n",sfx,channel);
#endif

		return channel;
	} // iv
	return -1;
} /* SFXManager::SFX_play_channel */


int SFXManager::SFX_play_channel(char *sfx, int channel, int volume, int angle, int distance)
{
	Mix_Chunk *s = get(sfx);

	if (s!=0) {
		Mix_PlayChannel(channel, s, 0);
		Mix_Volume(channel, volume);
		Mix_SetPosition(channel, angle, distance);

#ifdef __DEBUG_MESSAGES
	    output_debug_message("SFXManager::SFX_play_channel(%s), playing SFX in channel %i\n",sfx,channel);
#endif

		return channel;
	} // iv
	return -1;
} /* SFXManager::SFX_play_channel */


int SFXManager::SFX_play_continuous(char *a_sfx, int volume, void *object)
{
	Mix_Chunk *s = get(a_sfx);

	if (s!=0) {
		ContinuousSFX *sfx = new ContinuousSFX();
		sfx->m_object = object;
		sfx->m_channel = Mix_PlayChannel(-1, s, -1);
		Mix_Volume(sfx->m_channel, volume);
		m_continuous_being_played.Add(sfx);

#ifdef __DEBUG_MESSAGES
	    output_debug_message("SFXManager::SFX_play_continuous(%s), playing SFX in channel %i\n",a_sfx,sfx->m_channel);
#endif

		return sfx->m_channel;
	} // if
	return -1;
} /* SFXManager::SFX_play_continuous */


int SFXManager::SFX_play_continuous(char *a_sfx, int volume, int angle, int distance, void *object)
{
	Mix_Chunk *s = get(a_sfx);

	if (s!=0) {
		ContinuousSFX *sfx = new ContinuousSFX();
		sfx->m_object = object;
		sfx->m_channel = Mix_PlayChannel(-1, s, -1);
		Mix_Volume(sfx->m_channel, volume);
		Mix_SetPosition(sfx->m_channel, angle, distance);
		m_continuous_being_played.Add(sfx);

#ifdef __DEBUG_MESSAGES
	    output_debug_message("SFXManager::SFX_play_continuous(%s), playing SFX in channel %i\n",a_sfx,sfx->m_channel);
#endif

		return sfx->m_channel;
	} // if
	return -1;
} /* SFXManager::SFX_play_continuous */


void SFXManager::SFX_stop(int channel)
{
	ContinuousSFX *sfx,*todelete = 0;

	m_continuous_being_played.Rewind();
	while(todelete==0 && m_continuous_being_played.Iterate(sfx)) {
		if (sfx->m_channel == channel) todelete = sfx;
	} // while

	if (todelete!=0) {		
		Mix_HaltChannel(todelete->m_channel);
		m_continuous_being_played.DeleteElement(todelete);

#ifdef __DEBUG_MESSAGES
	    output_debug_message("SFXManager::SFX_stop(%i)\n",todelete->m_channel);
#endif

		delete todelete;
	} // if
} /* SFXManager::SFX_stop */


void SFXManager::SFX_stop_continuous(void)
{
	ContinuousSFX *sfx;

#ifdef __DEBUG_MESSAGES
	    output_debug_message("SFXManager::SFX_stop_continuous\n");
#endif

	while(!m_continuous_being_played.EmptyP()) {
		sfx = m_continuous_being_played.ExtractIni();
		Mix_HaltChannel(sfx->m_channel);

#ifdef __DEBUG_MESSAGES
	    output_debug_message("SFXManager::SFX_stop_continuous, stopping channel %i\n",sfx->m_channel);
#endif

		delete sfx;
	} // while
} /* SFXManager::SFX_stop_continuous */


void SFXManager::SFX_pause_continuous(void)
{
	ContinuousSFX *sfx;

	m_continuous_being_played.Rewind();
	while(m_continuous_being_played.Iterate(sfx)) {
		Mix_Pause(sfx->m_channel);
	} // while
} /* SFXManager::SFX_pause_continuous */


void SFXManager::SFX_resume_continuous(void)
{
	ContinuousSFX *sfx;

	m_continuous_being_played.Rewind();
	while(m_continuous_being_played.Iterate(sfx)) {
		Mix_Resume(sfx->m_channel);
	} // while
} /* SFXManager::SFX_resume_continuous */


