#ifndef _MUSIC_MANAGER
#define _MUSIC_MANAGER


class Music
{
public:
		int m_ID;
		int m_channel,m_previous_channel;	/* previous channel stores the old song in a crossfade */
		int m_volume;
		bool m_fade_in,m_fade_out;
		int m_fade_time,m_fade_timmer;
		bool m_paused;
};



class MusicManager : public SoundManager
{
    public:
        MusicManager();
        virtual ~MusicManager();

        void next_cycle(void);
		virtual void object_destroyed_notification(void *object);

		// "music_play" simply starts a new music. If a previous music with the same ID exists, then it stops it, and replaces
		// it with the new one. (music IDs are used to be able to ahve several musics playing at the same time, such as the
		// ambient sound in the goonies at the same time than the music).
		void music_play(int ID,char *file,int volume,int loops);
		void music_fadein(int ID,char *file,int volume,int loops,int time);
		void music_fadeout(int ID,int time);
		void music_volume(int ID,int volume);
		void music_stop(int ID);
		void music_stop(void);
		void music_pause(int ID);
		void music_pause(void);
		void music_resume(int ID);
		void music_resume(void);

    protected:

		virtual void channel_finished(int channel);

		List<Music> m_music_being_played;
};

#endif


