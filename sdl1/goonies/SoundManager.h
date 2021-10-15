#ifndef _SOUND_MANAGER
#define _SOUND_MANAGER


class SoundManagerNode
{
    public:
		~SoundManagerNode();

        Mix_Chunk *m_sfx;
        Symbol *m_name;
};


class SoundManager
{
	friend void channel_finished_cb(int channel);

    public:
        SoundManager();
        virtual ~SoundManager();

        Mix_Chunk *get(char *name);
        Mix_Chunk *get(Symbol *name);

		// new cycle of the game, do whatever your sound manager has to do with this event
        virtual void next_cycle(void); 

		// An object in the game has been destroyed, this function has to do whatever is associated with that event
		// like stopping a continuous SFX, etc.
		virtual void object_destroyed_notification(void *object);

        void cache(char *dirname);

    protected:
		Mix_Chunk *load_sound(char *file);
		int file_check(char *fname);
        char *remove_extension(char *filename);

		virtual void channel_finished(int channel);

        List<SoundManagerNode> *m_hash;
};

#endif


