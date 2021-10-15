#include <stdlib.h>
#include <stdarg.h>
//#include <sys/time.h>
#include "h2def.h"
#include "r_local.h"
#include "p_local.h"    // for P_AproxDistance
#include "sounds.h"
#include "i_sound.h"
#include "soundst.h"
#include "st_start.h"

#define	DEFAULT_ARCHIVEPATH ""


#define stricmp strcasecmp
#define PRIORITY_MAX_ADJUST 10
#define DIST_ADJUST (MAX_SND_DIST/PRIORITY_MAX_ADJUST)

extern void **lumpcache;


/*
===============================================================================

		MUSIC & SFX API

===============================================================================
*/

extern sfxinfo_t S_sfx[];
extern musicinfo_t S_music[];

static channel_t SChannel[MAX_CHANNELS];
static int RegisteredSong; //the current registered song.
static int NextCleanup;
static boolean MusicPaused;
static int Mus_Song = -1;
static int Mus_LumpNum;
static void *Mus_SndPtr;
static byte *SoundCurve;

static boolean UseSndScript;
static char ArchivePath[128];

extern int snd_MusicDevice;
extern int snd_SfxDevice;
extern int snd_MaxVolume;
extern int snd_MusicVolume;
extern int snd_Channels;

extern int startepisode;
extern int startmap;

// int AmbChan;

boolean S_StopSoundID(int sound_id, int priority);

//==========================================================================
//
// S_Start
//
//==========================================================================

void S_Start(void)
{
	S_StopAllSound();
	S_StartSong(gamemap, true);
}

//==========================================================================
//
// S_StartSong
//
//==========================================================================

void S_StartSong(int song, boolean loop)
{
	char *songLump;
	int size=0;
	{
		if(song == Mus_Song)
		{ // don't replay an old song
			return;
		}
		if(RegisteredSong)
		{
			I_StopSong(RegisteredSong);
			I_UnRegisterSong(RegisteredSong);
			if(UseSndScript)
			{
				Z_Free(Mus_SndPtr);
			}
			else
			{
				Z_ChangeTag(lumpcache[Mus_LumpNum], PU_CACHE);
			} 
			RegisteredSong = 0;
		}
		songLump = P_GetMapSongLump(song);
		if(!songLump)
		{
			return;
		}
		if(UseSndScript)
		{
			char name[128];
			sprintf(name, "%s%s.lmp", ArchivePath, songLump);
			M_ReadFile(name, (byte **)&Mus_SndPtr);
		}
		else
		{
			Mus_LumpNum = W_GetNumForName(songLump);
			size=W_LumpLength(Mus_LumpNum);
			Mus_SndPtr = W_CacheLumpNum(Mus_LumpNum, PU_MUSIC);
		} 
		if (size) {
  		 RegisteredSong = I_RegisterSong(Mus_SndPtr,size);
		 I_PlaySong(RegisteredSong, loop); // 'true' denotes endless looping.
		 Mus_Song = song;
		}
	}
}

//==========================================================================
//
// S_StartSongName
//
//==========================================================================

void S_StartSongName(char *songLump, boolean loop)
{
	int size=0;
	if(!songLump)
	{
		return;
	}

	{
		if(RegisteredSong)
		{
			I_StopSong(RegisteredSong);
			I_UnRegisterSong(RegisteredSong);
			if(UseSndScript)
			{
				Z_Free(Mus_SndPtr);
			}
			else
			{
				Z_ChangeTag(lumpcache[Mus_LumpNum], PU_CACHE);
			} 
			RegisteredSong = 0;
		}
		if(UseSndScript)
		{
			char name[128];
			sprintf(name, "%s%s.lmp", ArchivePath, songLump);
			M_ReadFile(name, (byte **)&Mus_SndPtr);
		}
		else
		{
			Mus_LumpNum = W_GetNumForName(songLump);
			size=W_LumpLength(Mus_LumpNum);
			Mus_SndPtr = W_CacheLumpNum(Mus_LumpNum, PU_MUSIC);
		} 
		RegisteredSong = I_RegisterSong(Mus_SndPtr,size);
		I_PlaySong(RegisteredSong, loop); // 'true' denotes endless looping.
		Mus_Song = -1;
	}
}

//==========================================================================
//
// S_GetSoundID
//
//==========================================================================

int S_GetSoundID(char *name)
{
	int i;

	for(i = 0; i < NUMSFX; i++)
	{
		if(!strcmp(S_sfx[i].tagName, name))
		{
			return i;
		}
	}
	return 0;
}

//==========================================================================
//
// S_StartSound
//
//==========================================================================

void S_StartSound(mobj_t *origin, int sound_id)
{
	S_StartSoundAtVolume(origin, sound_id, 127);
}

//==========================================================================
//
// S_StartSoundAtVolume
//
//==========================================================================
#if 0
void S_StartSoundAtVolume(mobj_t *origin, int sound_id, int volume)
{
	int dist, vol;
	int i = 0;       /* jim - initialise to avoid gcc warning */
	int priority;
	int sep;
	int angle;
	int absx;
	int absy;

	static int sndcount = 0;
	int chan;

    //printf( "S_StartSoundAtVolume: %d\n", sound_id );

	if(sound_id == 0 || snd_MaxVolume == 0)
		return;
	if(origin == NULL)
	{
		origin = players[displayplayer].mo;
	}
	if(volume == 0)
	{
		return;
	}

    // How does the DOS version work without this check?
    // players[0].mo does not get set until P_SpawnPlayer. - KR

    if( origin )
    {
        // calculate the distance before other stuff so that we can throw out
        // sounds that are beyond the hearing range.
        absx = abs(origin->x-players[displayplayer].mo->x);
        absy = abs(origin->y-players[displayplayer].mo->y);
        dist = absx+absy-(absx > absy ? absy>>1 : absx>>1);
        dist >>= FRACBITS;
        if(dist >= MAX_SND_DIST)
        {
          return; // sound is beyond the hearing range...
        }
        if(dist < 0)
        {
            dist = 0;
        }
        priority = S_sfx[sound_id].priority;
        priority *= (PRIORITY_MAX_ADJUST-(dist/DIST_ADJUST));
        if(!S_StopSoundID(sound_id, priority))
        {
            return; // other sounds have greater priority
        }
        for(i = 0; i<snd_Channels; i++)
        {
            if(origin->player)
            {
                i = snd_Channels;
                break; // let the player have more than one sound.
            }
            if(origin == SChannel[i].mo)
            { // only allow other mobjs one sound
                S_StopSound(SChannel[i].mo);
                break;
            }
        }
	}
    else
    {
        dist = 0;
        priority = S_sfx[sound_id].priority;
        if( ! S_StopSoundID( sound_id, priority ) )
        {
            return; // other sounds have greater priority
        }
    }

	if(i >= snd_Channels)
	{
		for(i = 0; i < snd_Channels; i++)
		{
			if(SChannel[i].mo == NULL)
			{
				break;
			}
		}
		if(i >= snd_Channels)
		{
			// look for a lower priority sound to replace.
			sndcount++;
			if(sndcount >= snd_Channels)
			{
				sndcount = 0;
			}
			for(chan = 0; chan < snd_Channels; chan++)
			{
				i = (sndcount+chan)%snd_Channels;
				if(priority >= SChannel[i].priority)
				{
					chan = -1; //denote that sound should be replaced.
					break;
				}
			}
			if(chan != -1)
			{
				return; //no free channels.
			}
			else //replace the lower priority sound.
			{
				if(SChannel[i].handle)
				{
					if(I_SoundIsPlaying(SChannel[i].handle))
					{
						I_StopSound(SChannel[i].handle);
					}
					if(S_sfx[SChannel[i].sound_id].usefulness > 0)
					{
						S_sfx[SChannel[i].sound_id].usefulness--;
					}
				}
			}
		}
	}
	if(S_sfx[sound_id].lumpnum == 0)
	{
		S_sfx[sound_id].lumpnum = I_GetSfxLumpNum(&S_sfx[sound_id]);
	}
	if(S_sfx[sound_id].snd_ptr == NULL)
	{
		if(UseSndScript)
		{
			char name[128];
			sprintf(name, "%s%s.lmp", ArchivePath, S_sfx[sound_id].lumpname);
			M_ReadFile(name, (byte **)&S_sfx[sound_id].snd_ptr);
		}
		else
		{
			S_sfx[sound_id].snd_ptr = W_CacheLumpNum(S_sfx[sound_id].lumpnum,
				PU_SOUND);
		} 
	}

	vol = (SoundCurve[dist]*(snd_MaxVolume*8)*volume)>>14;
	if(origin == players[displayplayer].mo)
	{
		sep = 128;
//              vol = (volume*(snd_MaxVolume+1)*8)>>7;
	}
	else
	{
#if 1
        // KR - SChannel[i].mo = 0 here!
        if( SChannel[i].mo == NULL )
        {
            sep = 128;
            //printf( " SChannel[i].mo not set\n" );
        }
        else
        {
#endif
		angle = R_PointToAngle2(players[displayplayer].mo->x,
			players[displayplayer].mo->y, SChannel[i].mo->x, SChannel[i].mo->y);
		angle = (angle-viewangle)>>24;
		sep = angle*2-128;
		if(sep < 64)
			sep = -sep;
		if(sep > 192)
			sep = 512-sep;
//              vol = SoundCurve[dist];
#if 1
        }
#endif
	}

	if(S_sfx[sound_id].changePitch)
	{
		SChannel[i].pitch = (byte)(127+(M_Random()&7)-(M_Random()&7));
	}
	else
	{
		SChannel[i].pitch = 127;
	}
	SChannel[i].handle = I_StartSound( sound_id, S_sfx[sound_id].snd_ptr, vol,
	                                  sep, SChannel[i].pitch, 0 );
	SChannel[i].mo = origin;
	SChannel[i].sound_id = sound_id;
	SChannel[i].priority = priority;
	SChannel[i].volume = volume;
	if(S_sfx[sound_id].usefulness < 0)
	{
		S_sfx[sound_id].usefulness = 1;
	}
	else
	{
		S_sfx[sound_id].usefulness++;
	}
}
#endif

void S_StartSoundAtVolume(mobj_t *origin, int sound_id, int volume)
{
	int dist, vol;
	int i;
	int priority;
	int sep;
	int angle;
	int absx;
	int absy;

	static int sndcount = 0;
	int chan;

	if(sound_id == 0 || snd_MaxVolume == 0)
		return;
#if 0
	if(origin == NULL)
	{
// origin = players[displayplayer].mo; bug -- can be uninitialized
	}
#endif
	if(volume == 0)
	{
		return;
	}

	// calculate the distance before other stuff so that we can throw out
	// sounds that are beyond the hearing range.
	if (origin)
	{
	absx = abs(origin->x-players[displayplayer].mo->x);
	absy = abs(origin->y-players[displayplayer].mo->y);
	}
	else
	  absx = absy = 0;
	dist = absx+absy-(absx > absy ? absy>>1 : absx>>1);
	dist >>= FRACBITS;
	if(dist >= MAX_SND_DIST)
	{
	  return; // sound is beyond the hearing range...
	}
	if(dist < 0)
	{
		dist = 0;
	}
	priority = S_sfx[sound_id].priority;
	priority *= (PRIORITY_MAX_ADJUST-(dist/DIST_ADJUST));
	if(!S_StopSoundID(sound_id, priority))
	{
		return; // other sounds have greater priority
	}
	for(i=0; i<snd_Channels; i++)
	{
		if(!origin || origin->player)
		{
			i = snd_Channels;
			break; // let the player have more than one sound.
		}
		if(origin == SChannel[i].mo)
		{ // only allow other mobjs one sound
			S_StopSound(SChannel[i].mo);
			break;
		}
	}
	if(i >= snd_Channels)
	{
		for(i = 0; i < snd_Channels; i++)
		{
			if(SChannel[i].mo == NULL)
			{
				break;
			}
		}
		if(i >= snd_Channels)
		{
			// look for a lower priority sound to replace.
			sndcount++;
			if(sndcount >= snd_Channels)
			{
				sndcount = 0;
			}
			for(chan = 0; chan < snd_Channels; chan++)
			{
				i = (sndcount+chan)%snd_Channels;
				if(priority >= SChannel[i].priority)
				{
					chan = -1; //denote that sound should be replaced.
					break;
				}
			}
			if(chan != -1)
			{
				return; //no free channels.
			}
			else //replace the lower priority sound.
			{
				if(SChannel[i].handle)
				{
					if(I_SoundIsPlaying(SChannel[i].handle))
					{
						I_StopSound(SChannel[i].handle);
					}
					if(S_sfx[SChannel[i].sound_id].usefulness > 0)
					{
						S_sfx[SChannel[i].sound_id].usefulness--;
					}
				}
			}
		}
	}
	if(S_sfx[sound_id].lumpnum == 0)
	{
		S_sfx[sound_id].lumpnum = I_GetSfxLumpNum(&S_sfx[sound_id]);
	}
	if(S_sfx[sound_id].snd_ptr == NULL)
	{
		if(UseSndScript)
		{
			char name[128];
			sprintf(name, "%s%s.lmp", ArchivePath, S_sfx[sound_id].lumpname);
			M_ReadFile(name, (byte **)&S_sfx[sound_id].snd_ptr);
		}
		else
		{
			S_sfx[sound_id].snd_ptr = W_CacheLumpNum(S_sfx[sound_id].lumpnum,
				PU_SOUND);
		}
		#ifdef __WATCOMC__
//		  _dpmi_lockregion(S_sfx[sound_id].snd_ptr,
//			  lumpinfo[S_sfx[sound_id].lumpnum].size);
		#endif
	}

	vol = (SoundCurve[dist]*(snd_MaxVolume*8)*volume)>>14;
	if (!origin || origin == players[displayplayer].mo)
	{
		sep = 128;
//              vol = (volume*(snd_MaxVolume+1)*8)>>7;
	}
	else
	{
		angle = R_PointToAngle2(players[displayplayer].mo->x,
// bug! 		       players[displayplayer].mo->y, SChannel[i].mo->x, SChannel[i].mo->y);
			players[displayplayer].mo->y, origin->x, origin->y);
		angle = (angle-viewangle)>>24;
		sep = angle*2-128;
		if(sep < 64)
			sep = -sep;
		if(sep > 192)
			sep = 512-sep;
//              vol = SoundCurve[dist];
	}

	if(S_sfx[sound_id].changePitch)
	{
		SChannel[i].pitch = (byte)(127+(M_Random()&7)-(M_Random()&7));
	}
	else
	{
		SChannel[i].pitch = 127;
	}
	SChannel[i].handle = I_StartSound(sound_id, S_sfx[sound_id].snd_ptr, vol,
		sep, SChannel[i].pitch, 0);
	SChannel[i].mo = origin;
	SChannel[i].sound_id = sound_id;
	SChannel[i].priority = priority;
	SChannel[i].volume = volume;
	if(S_sfx[sound_id].usefulness < 0)
	{
		S_sfx[sound_id].usefulness = 1;
	}
	else
	{
		S_sfx[sound_id].usefulness++;
	}
}
//==========================================================================
//
// S_StopSoundID
//
//==========================================================================

boolean S_StopSoundID(int sound_id, int priority)
{
	int i;
	int lp; //least priority
	int found;

	if(S_sfx[sound_id].numchannels == -1)
	{
		return(true);
	}
	lp = -1; //denote the argument sound_id
	found = 0;
	for(i=0; i<snd_Channels; i++)
	{
		if(SChannel[i].sound_id == sound_id && SChannel[i].mo)
		{
			found++; //found one.  Now, should we replace it??
			if(priority >= SChannel[i].priority)
			{ // if we're gonna kill one, then this'll be it
				lp = i;
				priority = SChannel[i].priority;
			}
		}
	}
	if(found < S_sfx[sound_id].numchannels)
	{
		return(true);
	}
	else if(lp == -1)
	{
		return(false); // don't replace any sounds
	}
	if(SChannel[lp].handle)
	{
		if(I_SoundIsPlaying(SChannel[lp].handle))
		{
			I_StopSound(SChannel[lp].handle);
		}
		if(S_sfx[SChannel[lp].sound_id].usefulness > 0)
		{
			S_sfx[SChannel[lp].sound_id].usefulness--;
		}
		SChannel[lp].mo = NULL;
	}
	return(true);
}

//==========================================================================
//
// S_StopSound
//
//==========================================================================

void S_StopSound(mobj_t *origin)
{
	int i;

	for(i=0;i<snd_Channels;i++)
	{
		if(SChannel[i].mo == origin)
		{
			I_StopSound(SChannel[i].handle);
			if(S_sfx[SChannel[i].sound_id].usefulness > 0)
			{
				S_sfx[SChannel[i].sound_id].usefulness--;
			}
			SChannel[i].handle = 0;
			SChannel[i].mo = NULL;
		}
	}
}

//==========================================================================
//
// S_StopAllSound
//
//==========================================================================

void S_StopAllSound(void)
{
	int i;

	//stop all sounds
	for(i=0; i < snd_Channels; i++)
	{
		if(SChannel[i].handle)
		{
			S_StopSound(SChannel[i].mo);
		}
	}
	memset(SChannel, 0, 8*sizeof(channel_t));
}

//==========================================================================
//
// S_SoundLink
//
//==========================================================================

void S_SoundLink(mobj_t *oldactor, mobj_t *newactor)
{
	int i;

	for(i=0;i<snd_Channels;i++)
	{
		if(SChannel[i].mo == oldactor)
			SChannel[i].mo = newactor;
	}
}

//==========================================================================
//
// S_PauseSound
//
//==========================================================================

void S_PauseSound(void)
{
	{
		I_PauseSong(RegisteredSong);
	}
}

//==========================================================================
//
// S_ResumeSound
//
//==========================================================================

void S_ResumeSound(void)
{
	{
		I_ResumeSong(RegisteredSong);
	}
}

//==========================================================================
//
// S_UpdateSounds
//
//==========================================================================

void S_UpdateSounds(mobj_t *listener)
{
	int i, dist, vol;
	int angle;
	int sep;
	int priority;
	int absx;
	int absy;

	if(snd_MaxVolume == 0)
	{
		return;
	}

	// Update any Sequences
	SN_UpdateActiveSequences();

	if(NextCleanup < gametic)
	{
		if(UseSndScript)
		{
			for(i = 0; i < NUMSFX; i++)
			{
				if(S_sfx[i].usefulness == 0 && S_sfx[i].snd_ptr)
				{
					S_sfx[i].usefulness = -1;
				}
			}
		}
		else
		{
			for(i = 0; i < NUMSFX; i++)
			{
				if(S_sfx[i].usefulness == 0 && S_sfx[i].snd_ptr)
				{
					if(lumpcache[S_sfx[i].lumpnum])
					{
						if(((memblock_t *)((byte*)
							(lumpcache[S_sfx[i].lumpnum])-
							sizeof(memblock_t)))->id == 0x1d4a11)
						{ // taken directly from the Z_ChangeTag macro
							Z_ChangeTag2(lumpcache[S_sfx[i].lumpnum],
								PU_CACHE); 
						}
					}
					S_sfx[i].usefulness = -1;
					S_sfx[i].snd_ptr = NULL;
				}
			}
		}
		NextCleanup = gametic+35*30; // every 30 seconds
	}
	for(i=0;i<snd_Channels;i++)
	{
		if(!SChannel[i].handle || S_sfx[SChannel[i].sound_id].usefulness == -1)
		{
			continue;
		}
		if(!I_SoundIsPlaying(SChannel[i].handle))
		{
			if(S_sfx[SChannel[i].sound_id].usefulness > 0)
			{
				S_sfx[SChannel[i].sound_id].usefulness--;
			}
			SChannel[i].handle = 0;
			SChannel[i].mo = NULL;
			SChannel[i].sound_id = 0;
		}
		if(SChannel[i].mo == NULL || SChannel[i].sound_id == 0
			|| SChannel[i].mo == listener)
		{
			continue;
		}
		else
		{
			absx = abs(SChannel[i].mo->x-listener->x);
			absy = abs(SChannel[i].mo->y-listener->y);
			dist = absx+absy-(absx > absy ? absy>>1 : absx>>1);
			dist >>= FRACBITS;

			if(dist >= MAX_SND_DIST)
			{
				S_StopSound(SChannel[i].mo);
				continue;
			}
			if(dist < 0)
			{
				dist = 0;
			}
			//vol = SoundCurve[dist];
			vol = (SoundCurve[dist]*(snd_MaxVolume*8)*SChannel[i].volume)>>14;
			if(SChannel[i].mo == listener)
			{
				sep = 128;
			}
			else
			{
				angle = R_PointToAngle2(listener->x, listener->y,
								SChannel[i].mo->x, SChannel[i].mo->y);
				angle = (angle-viewangle)>>24;
				sep = angle*2-128;
				if(sep < 64)
					sep = -sep;
				if(sep > 192)
					sep = 512-sep;
			}
			I_UpdateSoundParams(SChannel[i].handle, vol, sep,
				SChannel[i].pitch);
			priority = S_sfx[SChannel[i].sound_id].priority;
			priority *= PRIORITY_MAX_ADJUST-(dist/DIST_ADJUST);
			SChannel[i].priority = priority;
		}
	}
}

//==========================================================================
//
// S_Init
//
//==========================================================================

void S_Init(void)
{
	SoundCurve = W_CacheLumpName("SNDCURVE", PU_STATIC);
//      SoundCurve = Z_Malloc(MAX_SND_DIST, PU_STATIC, NULL);
	I_StartupSound();
	if(snd_Channels > 8)
	{
		snd_Channels = 8;
	}
	I_SetChannels(snd_Channels);
	I_SetMusicVolume(snd_MusicVolume);

}

//==========================================================================
//
// S_GetChannelInfo
//
//==========================================================================

void S_GetChannelInfo(SoundInfo_t *s)
{
	int i;
	ChanInfo_t *c;

	s->channelCount = snd_Channels;
	s->musicVolume = snd_MusicVolume;
	s->soundVolume = snd_MaxVolume;
	for(i = 0; i < snd_Channels; i++)
	{
		c = &s->chan[i];
		c->id = SChannel[i].sound_id;
		c->priority = SChannel[i].priority;
		c->name = S_sfx[c->id].lumpname;
		c->mo = SChannel[i].mo;
		c->distance = (c->mo) ? (P_AproxDistance(c->mo->x-viewx, c->mo->y-viewy)
			>>FRACBITS) :0;
	}
}

//==========================================================================
//
// S_GetSoundPlayingInfo
//
//==========================================================================

boolean S_GetSoundPlayingInfo(mobj_t *mobj, int sound_id)
{
	int i;

	for(i = 0; i < snd_Channels; i++)
	{
		if(SChannel[i].sound_id == sound_id && SChannel[i].mo == mobj)
		{
			if(I_SoundIsPlaying(SChannel[i].handle))
			{
				return true;
			}
		}
	}
	return false;
}

//==========================================================================
//
// S_SetMusicVolume
//
//==========================================================================

void S_SetMusicVolume(void)
{
	{
		I_SetMusicVolume(snd_MusicVolume);
	}
	if(snd_MusicVolume == 0)
	{
		I_PauseSong(RegisteredSong);
		MusicPaused = true;
	}
	else if(MusicPaused)
	{
		I_ResumeSong(RegisteredSong);
		MusicPaused = false;
	}
}

//==========================================================================
//
// S_ShutDown
//
//==========================================================================

void S_ShutDown(void)
{
	extern int tsm_ID;
	if(tsm_ID != -1)
	{
		I_StopSong(RegisteredSong);
		I_UnRegisterSong(RegisteredSong);
		I_ShutdownSound();
	}
}

//==========================================================================
//
// S_InitScript
//
//==========================================================================

void S_InitScript(void)
{
	int p;
	int i;

	strcpy(ArchivePath, DEFAULT_ARCHIVEPATH);
	if(!(p = M_CheckParm("-devsnd")))
	{
		UseSndScript = false;
		SC_OpenLump("sndinfo");
	}
	else
	{
		UseSndScript = true;
		SC_OpenFile(myargv[p+1]);
	}
	while(SC_GetString())
	{
		if(*sc_String == '$')
		{
			if(!stricmp(sc_String, "$ARCHIVEPATH"))
			{
				SC_MustGetString();
				strcpy(ArchivePath, sc_String);
			}
			else if(!stricmp(sc_String, "$MAP"))
			{
				SC_MustGetNumber();
				SC_MustGetString();
				if(sc_Number)
				{
					P_PutMapSongLump(sc_Number, sc_String);
				}
			}
			continue;
		}
		else
		{
			for(i = 0; i < NUMSFX; i++)
			{
				if(!strcmp(S_sfx[i].tagName, sc_String))
				{
					SC_MustGetString();
					if(*sc_String != '?')
					{
						strcpy(S_sfx[i].lumpname, sc_String);
					}
					else
					{
						strcpy(S_sfx[i].lumpname, "default");
					}
					break;
				}
			}
			if(i == NUMSFX)
			{
				SC_MustGetString();
			}
		}
	}
	SC_Close();

	for(i = 0; i < NUMSFX; i++)
	{
		if(!strcmp(S_sfx[i].lumpname, ""))
		{
			strcpy(S_sfx[i].lumpname, "default");
		}
	}
}

