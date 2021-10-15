#ifndef _THE_GOONIES_OBJECT_MUSICALNOTE
#define _THE_GOONIES_OBJECT_MUSICALNOTE

class GO_musicalnote : public GO_enemy
{
    public:
        GO_musicalnote(int x, int y, int sfx_volume, float speedx, float speedy);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual int enemy_hit(void);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
        float m_speedx, m_speedy;
};

#endif

