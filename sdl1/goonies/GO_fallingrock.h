#ifndef _THE_GOONIES_OBJECT_FALLINGROCK
#define _THE_GOONIES_OBJECT_FALLINGROCK

class GO_fallingrock : public GO_enemy
{
    public:
        GO_fallingrock(int x, int y, int sfx_volume, int color);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual int enemy_hit(void);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
        int m_color;  // 0 : normal, 1: blue, 2: green

};

#endif

