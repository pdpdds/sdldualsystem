#ifndef _THE_GOONIES_OBJECT_CLOSINGWALL
#define _THE_GOONIES_OBJECT_CLOSINGWALL

class GO_closingwall : public GObject
{
    public:
        GO_closingwall(int x, int y, int sfx_volume, int color);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
        int m_color;  // 0 : normal, 1: blue, 2: green, 3: yellow
};

#endif

