#ifndef _THE_GOONIES_OBJECT_FRATELLI
#define _THE_GOONIES_OBJECT_FRATELLI

class GO_fratelli : public GO_enemy
{
    public:
        GO_fratelli(int x, int y, int sfx_volume, int color);
        virtual ~GO_fratelli();

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool player_hit(int *experience, int *score);
        virtual int enemy_hit(void);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
        virtual bool internal_cycle(VirtualController *k, GMap *map, int layer, GLTManager *GLTM, SFXManager *SFXM);

        int m_walking_channel;
        int m_climbing_channel;

        VirtualController *m_vc;

        float m_walking_speed;

        int m_color;

        int m_hit_counter;
        int m_time_since_last_bullet;

        int m_AI_state;
        int m_AI_state_cycle;

        int m_turning_counter;
        int m_previous_state;

};

#endif

