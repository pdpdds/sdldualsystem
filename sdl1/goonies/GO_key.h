#ifndef _THE_GOONIES_OBJECT_KEY
#define _THE_GOONIES_OBJECT_KEY

class GO_key : public GObject
{
    public:
        GO_key(int x, int y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
};

#endif

