#ifndef _THE_GOONIES_OBJECT_ITEM
#define _THE_GOONIES_OBJECT_ITEM

class GO_item : public GObject
{
    public:
        GO_item(int x, int y, int sfx_volume, int type);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

        int get_type(void)
        {
            return m_type;
        };

    protected:
        int m_type;
};

#endif

