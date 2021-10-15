#ifndef _THE_GOONIES_OBJECT_WATERSPLASH
#define _THE_GOONIES_OBJECT_WATERSPLASH

class GO_watersplash : public GObject
{
    public:
        GO_watersplash(int x, int y, float speed_x, float speed_y, int sfx_volume);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);


    protected:

        int m_dx, m_dy;
        float m_speed_x, m_speed_y;
};

#endif

