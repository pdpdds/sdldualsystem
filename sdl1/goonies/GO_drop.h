#ifndef _THE_GOONIES_OBJECT_DROP
#define _THE_GOONIES_OBJECT_DROP

class GO_drop : public GO_enemy
{
    public:
        GO_drop(int x, int y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual int enemy_hit(void);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
};

#endif

