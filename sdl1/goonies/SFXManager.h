#ifndef _SFX_MANAGER
#define _SFX_MANAGER


class ContinuousSFX
{
public:
		int m_channel;
		void *m_object;
};


class SFXManager : public SoundManager
{
    public:
        SFXManager();
        virtual ~SFXManager();

        virtual void next_cycle(void);
		virtual void object_destroyed_notification(void *object);

		int SFX_play(char *sfx, int volume);
		int SFX_play(char *sfx, int volume, int angle, int distance);
		int SFX_play_channel(char *sfx, int channel, int volume);
		int SFX_play_channel(char *sfx, int channel, int volume, int angle, int distance);

		// The continuous SFX require an "object" to be passed so that the SoundManager can determine if the object that
		// started them still exists, and stop them otherwise:
		int SFX_play_continuous(char *sfx, int volume, void *object);
		int SFX_play_continuous(char *sfx, int volume, int angle, int distance, void *object);
		void SFX_stop(int channel);

		void SFX_stop_continuous(void);
		void SFX_pause_continuous(void);
		void SFX_resume_continuous(void);

    protected:
		virtual void channel_finished(int channel);

        List<Mix_Chunk> m_already_played;
		List<ContinuousSFX> m_continuous_being_played;
};

#endif


