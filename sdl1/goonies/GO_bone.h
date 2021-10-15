#ifndef _THE_GOONIES_OBJECT_BONE
#define _THE_GOONIES_OBJECT_BONE

class GO_bone : public GO_enemy
{
    public:
        GO_bone(int x, int y, int sfx_volume, int facing);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool player_hit(int *experience, int *score);
        virtual int enemy_hit(void);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
        int m_facing;
};

#endif

