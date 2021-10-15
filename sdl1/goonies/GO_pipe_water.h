#ifndef _THE_GOONIES_OBJECT_PIPE_WATER
#define _THE_GOONIES_OBJECT_PIPE_WATER

class GO_pipe_water : public GO_enemy
{
    public:
        GO_pipe_water(int x, int y, int sfx_volume, int facing);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool player_hit(int *experience, int *score);
        virtual int enemy_hit(void);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);


    protected:
        int m_facing;
        int m_wait_cycles;
        int m_pipe_channel;
};

#endif

