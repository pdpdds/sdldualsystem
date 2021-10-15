#ifndef _THE_GOONIES_OBJECT_WATER
#define _THE_GOONIES_OBJECT_WATER

class GO_water : public GObject
{
    public:
        GO_water(int x, int y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

        void set_map(class GMap *m)
        {
            m_map = m;
        };

    protected:

        GMap *m_map;
};

#endif

