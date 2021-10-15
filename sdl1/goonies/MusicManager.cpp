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
#include "MusicManager.h"

// #include "debug.h"



void MusicManager::channel_finished(int channel)
{
	Music *m,*newm=0;

	m_music_being_played.Rewind();
	while(newm==0 && m_music_being_played.Iterate(m)) {
		if (m->m_channel==channel) newm=m;
	} // while

	if (newm!=0) {
		m_music_being_played.DeleteElement(newm);
		delete newm;
	} // if
} /* MusicManager::channel_finished */




MusicManager::MusicManager()
{
} /* MusicManager::MusicManager */


MusicManager::~MusicManager()
{
} /* MusicManager::~MusicManager */


void MusicManager::next_cycle(void)
{
	// Music fade in/out:
	{
		Music *m;
		List<Music> to_delete;

		m_music_being_played.Rewind();
		while(m_music_being_played.Iterate(m)) {
			if (!m->m_paused) {
				if (m->m_fade_in) {
					if (m->m_fade_timmer>=m->m_fade_time) {
						Mix_Volume(m->m_channel,m->m_volume);
						m->m_fade_in=false;

						if (m->m_previous_channel!=-1) {
							Mix_HaltChannel(m->m_previous_channel);
							m->m_previous_channel=-1;
						} // if
					} else {
						int volume = int(m->m_volume*(float(m->m_fade_timmer)/float(m->m_fade_time)));
						Mix_Volume(m->m_channel,volume);
						if (m->m_previous_channel!=-1) {
							Mix_Volume(m->m_previous_channel,m->m_volume-volume);
						} // if
						m->m_fade_timmer++;
					} // if
				} // if

				if (m->m_fade_out) {
					if (m->m_fade_timmer>=m->m_fade_time) {
						to_delete.Add(m);
					} else {
						int volume = int(m->m_volume*(float(m->m_fade_time - m->m_fade_timmer)/float(m->m_fade_time)));
						Mix_Volume(m->m_channel,volume);
						if (m->m_previous_channel!=-1) {
							Mix_HaltChannel(m->m_previous_channel);
							m->m_previous_channel=-1;
						} // if
						m->m_fade_timmer++;
					} // if
				} // if
			} // if
		} // while

		while(!to_delete.EmptyP()) {
			m = to_delete.ExtractIni();
			music_stop(m->m_ID);
		} // while
	}

} /* MusicManager::next_cycle */


void MusicManager::object_destroyed_notification(void *object)
{
}

void MusicManager::music_play(int ID,char *file,int volume,int loops)
{
	Music *m,*newm=0;
	Mix_Chunk *s = get(file);

	if (s!=0) {
		m_music_being_played.Rewind();
		while(newm==0 && m_music_being_played.Iterate(m)) {
			if (m->m_ID==ID) newm=m;
		} // while

		if (newm==0) {
			newm = new Music();
			newm->m_channel = -1;
			m_music_being_played.Add(newm);
		} else {
			if (newm->m_previous_channel!=-1) Mix_HaltChannel(newm->m_previous_channel);
		} // if

		// We delete it from the list so that when the previous music is stopped (by Mix_PlayChannel, the stop_music
		// method that will be automatically called by the call back does not delete this nod from the list:
		m_music_being_played.DeleteElement(newm);
		newm->m_ID=ID;
		newm->m_channel = Mix_PlayChannel( newm->m_channel, s, loops);
		Mix_Volume(newm->m_channel, volume);
		newm->m_previous_channel=-1;
		newm->m_volume=volume;
		newm->m_fade_in=false;
		newm->m_fade_out=false;
		newm->m_fade_time=0;
		newm->m_fade_timmer=0;
		newm->m_paused=false;
		m_music_being_played.Add(newm);

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_play(%i), playing music ID %i (channels %i,%i)\n",ID,newm->m_ID,newm->m_channel,newm->m_previous_channel);
#endif

	} // if
} /* MusicManager::music_play */


void MusicManager::music_fadein(int ID,char *file,int volume,int loops,int time)
{
	Music *m,*newm=0;
	Mix_Chunk *s = get(file);

	if (s!=0) {
		m_music_being_played.Rewind();
		while(newm==0 && m_music_being_played.Iterate(m)) {
			if (m->m_ID==ID) newm=m;
		} // while

		if (newm==0) {
			newm = new Music();
			newm->m_previous_channel=-1;
			m_music_being_played.Add(newm);
		} else {
			if (newm->m_previous_channel!=-1) Mix_HaltChannel(newm->m_previous_channel);
			newm->m_previous_channel=newm->m_channel;
		} // if

		newm->m_ID=ID;
		newm->m_volume=volume;
		newm->m_fade_in=true;
		newm->m_fade_out=false;
		newm->m_fade_time=time;
		newm->m_fade_timmer=0;
		newm->m_paused=false;
		{
			int volume = int(newm->m_volume*(float(newm->m_fade_timmer)/float(newm->m_fade_time)));
			newm->m_channel = Mix_PlayChannel( -1, s, loops);
			Mix_Volume(newm->m_channel, volume);
			if (newm->m_channel==newm->m_previous_channel) newm->m_previous_channel=-1;
			if (newm->m_previous_channel!=-1) {
				Mix_Volume(newm->m_previous_channel,newm->m_volume-volume);
			} // if
		}

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_fadein(%i,%i), fading in music ID %i (channels %i,%i)\n",ID,time,newm->m_ID,newm->m_channel,newm->m_previous_channel);
#endif

	} // if
} /* MusicManager::music_fadein */


void MusicManager::music_fadeout(int ID,int time)
{
	Music *m,*newm=0;

	m_music_being_played.Rewind();
	while(newm==0 && m_music_being_played.Iterate(m)) {
		if (m->m_ID==ID) newm=m;
	} // while

	if (newm!=0) {
		newm->m_fade_in=false;
		newm->m_fade_out=true;
		newm->m_fade_time=time;
		newm->m_fade_timmer=0;

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_fadeout(%i,%i), fading out music ID %i (channels %i,%i)\n",ID,time,newm->m_ID,newm->m_channel,newm->m_previous_channel);
#endif

	} // if
} /* MusicManager::music_fadeout */


void MusicManager::music_volume(int ID,int volume)
{
	Music *m,*newm=0;

	m_music_being_played.Rewind();
	while(newm==0 && m_music_being_played.Iterate(m)) {
		if (m->m_ID==ID) {
			m->m_volume=volume;
			if (!m->m_fade_in && !m->m_fade_out) {
				Mix_Volume(m->m_channel,m->m_volume);
			} // if
		} // if
	} // while
} /* MusicManager::music_volume */


void MusicManager::music_stop(int ID)
{
	Music *m,*newm=0;

	m_music_being_played.Rewind();
	while(newm==0 && m_music_being_played.Iterate(m)) {
		if (m->m_ID==ID) newm=m;
	} // while

	if (newm!=0) {

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_stop(ID), stopping music ID %i (channels %i,%i)\n",newm->m_ID,newm->m_channel,newm->m_previous_channel);
#endif

		m_music_being_played.DeleteElement(newm);
		Mix_HaltChannel(newm->m_channel);
		if (newm->m_previous_channel!=-1) Mix_HaltChannel(newm->m_previous_channel);
		delete newm;
	} // if
} /* MusicManager::music_stop */


void MusicManager::music_stop(void)
{
	Music *m;

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_stop\n");
#endif

	while(!m_music_being_played.EmptyP()) {
		m=m_music_being_played.ExtractIni();

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_stop, stopping music ID %i (channels %i,%i)\n",m->m_ID,m->m_channel,m->m_previous_channel);
#endif

		Mix_HaltChannel(m->m_channel);
		if (m->m_previous_channel!=-1) Mix_HaltChannel(m->m_previous_channel);
		delete m;		
	} // while

} /* MusicManager::music_stop */


void MusicManager::music_pause(int ID)
{
	Music *m,*newm=0;

	m_music_being_played.Rewind();
	while(newm==0 && m_music_being_played.Iterate(m)) {
		if (m->m_ID==ID) {
			Mix_Pause(m->m_channel);
			if (m->m_previous_channel!=-1) Mix_Pause(m->m_previous_channel);
			m->m_paused=true;

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_pause(ID), pausing music ID %i (channels %i,%i)\n",m->m_ID,m->m_channel,m->m_previous_channel);
#endif

			newm=m;
		} // if
	} // while

} /* MusicManager::music_pause */


void MusicManager::music_pause(void)
{
	Music *m;

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_pause\n");
#endif

	m_music_being_played.Rewind();
	while(m_music_being_played.Iterate(m)) {
		Mix_Pause(m->m_channel);
		if (m->m_previous_channel!=-1) Mix_Pause(m->m_previous_channel);
		m->m_paused=true;

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_pause, pausing music ID %i (channels %i,%i)\n",m->m_ID,m->m_channel,m->m_previous_channel);
#endif

	} // while
} /* MusicManager::music_pause */


void MusicManager::music_resume(int ID)
{
	Music *m,*newm=0;

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_resume(ID)\n");
#endif

	m_music_being_played.Rewind();
	while(newm==0 && m_music_being_played.Iterate(m)) {
		if (m->m_ID==ID) {
			Mix_Resume(m->m_channel);
			if (m->m_previous_channel!=-1) Mix_Resume(m->m_previous_channel);
			m->m_paused=false;

#ifdef __DEBUG_MESSAGES
		   output_debug_message("MusicManager::music_resume(ID), resuming music ID %i (channels %i,%i)\n",m->m_ID,m->m_channel,m->m_previous_channel);
#endif

			newm=m;
		} // if
	} // while
} /* MusicManager::music_resume */


void MusicManager::music_resume(void)
{
	Music *m;

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_resume\n");
#endif

	m_music_being_played.Rewind();
	while(m_music_being_played.Iterate(m)) {
		Mix_Resume(m->m_channel);
		if (m->m_previous_channel!=-1) Mix_Resume(m->m_previous_channel);
		m->m_paused=false;

#ifdef __DEBUG_MESSAGES
	    output_debug_message("MusicManager::music_resume, resuming music ID %i (channels %i,%i)\n",m->m_ID,m->m_channel,m->m_previous_channel);
#endif

	} // while
} /* MusicManager::music_resume */


