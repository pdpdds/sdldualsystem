#ifndef _THE_GOONIES_ENEMY
#define _THE_GOONIES_ENEMY

class GO_enemy : public GObject
{
    public:
        GO_enemy(int x, int y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool player_hit(int *experience, int *score); // called when the player hits the enemy (returns the amount of experience gained)
        virtual int enemy_hit(void);    // called when the enemy hits the player

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
};

#endif

