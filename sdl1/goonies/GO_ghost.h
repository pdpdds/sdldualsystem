#ifndef _THE_GOONIES_OBJECT_GHOST
#define _THE_GOONIES_OBJECT_GHOST

class GO_ghost : public GO_enemy
{
    public:
        GO_ghost(int x, int y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool player_hit(int *experience, int *score);
        virtual int enemy_hit(void);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
        float m_original_x, m_original_y;
        int m_energy;
        int m_movement_state;
        int m_hit_timmer;
        float m_destination_x, m_destination_y;
        float m_radius;
};

#endif

