#ifndef _THE_GOONIES_OBJECT_FALLINGWATER
#define _THE_GOONIES_OBJECT_FALLINGWATER

class GO_fallingwater : public GO_enemy
{
    public:
        GO_fallingwater(int x, int y, int dx, int dy, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

        virtual int enemy_hit(void);

        void set_map(class GMap *m)
        {
            m_map = m;
        };

        virtual bool collision(GObject *o);

    protected:

        int m_type;  // 0 water, 1 lava
        GMap *m_map;
        int m_dx, m_dy;
        float m_speed;
};

#endif

