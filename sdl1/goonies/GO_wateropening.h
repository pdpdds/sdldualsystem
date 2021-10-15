#ifndef _THE_GOONIES_OBJECT_WATEROPENING
#define _THE_GOONIES_OBJECT_WATEROPENING

class GO_wateropening : public GObject
{
    public:
        GO_wateropening(int x, int y, int sfx_volume, int color);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:

        int m_color;
        char *watersfx;
        int m_water_channel;
		int m_water_volume;
};

#endif

