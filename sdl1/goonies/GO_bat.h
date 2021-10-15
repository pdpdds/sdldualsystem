#ifndef _THE_GOONIES_OBJECT_BAT
#define _THE_GOONIES_OBJECT_BAT

class GO_bat : public GO_enemy
{
    public:
        GO_bat(int x, int y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool player_hit(int *experience, int *score);
        virtual int enemy_hit(void);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
		int m_flying_channel;

        int m_cave_x, m_cave_y;
        int m_original_y;
};

#endif

