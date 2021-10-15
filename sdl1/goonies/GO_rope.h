#ifndef _THE_GOONIES_OBJECT_ROPE
#define _THE_GOONIES_OBJECT_ROPE

class GO_rope : public GObject
{
    public:
        GO_rope(int x, int y, int sfx_volume, int type); // type = 0 for top rope, = 1 for middle rope and = 2 for bottom rope

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
        int m_type;
};

#endif

