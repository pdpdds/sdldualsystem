#ifndef _THE_GOONIES_OBJECT_ENTRYDOOR
#define _THE_GOONIES_OBJECT_ENTRYDOOR

class GO_entrydoor : public GObject
{
    public:
        GO_entrydoor(int x, int y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
};

#endif

