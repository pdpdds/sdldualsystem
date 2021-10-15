#ifndef _THE_GOONIES_OBJECT_CAGEDOOR
#define _THE_GOONIES_OBJECT_CAGEDOOR

class GO_cagedoor : public GObject
{
    public:
        GO_cagedoor(int x, int y, int sfx_volume, int n_locks, int item);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

    protected:
        int m_item;
        int m_n_locks;
        int m_state_cycle;
		GLTile *m_lock;
};

#endif

