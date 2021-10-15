#ifndef _THE_GOONIES_OBJECT_FLAME
#define _THE_GOONIES_OBJECT_FLAME

class GO_flame : public GO_enemy
{
    public:
        GO_flame(int x, int y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual int enemy_hit(void);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
        int m_flame_channel;
};

#endif

