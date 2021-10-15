#ifndef _THE_GOONIES_OBJECT_LAVA
#define _THE_GOONIES_OBJECT_LAVA

class GO_lava : public GObject
{
    public:
        GO_lava(int x, int y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

        void set_map(class GMap *l)
        {
            m_map = l;
        };

    protected:

        GMap *m_map;
};

#endif

