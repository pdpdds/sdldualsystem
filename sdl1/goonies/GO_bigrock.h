#ifndef _THE_GOONIES_OBJECT_BIGROCK
#define _THE_GOONIES_OBJECT_BIGROCK

class GO_bigrock : public GObject
{
    public:
        GO_bigrock(int x, int y, int sfx_volume, int color);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:

        int m_color;

        int m_n_fall;   // counts how many times the rock has fallen
        int m_top_y, m_middle_y;
        int m_original_y;
        int m_chain_channel;
		GLTile *m_chain;
};

#endif

