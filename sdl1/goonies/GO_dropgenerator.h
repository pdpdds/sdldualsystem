#ifndef _THE_GOONIES_OBJECT_DROPGENERATOR
#define _THE_GOONIES_OBJECT_DROPGENERATOR

class GO_dropgenerator : public GObject
{
    public:
        GO_dropgenerator(int x, int y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
};

#endif

