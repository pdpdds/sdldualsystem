#ifndef _THE_GOONIES_OBJECT
#define _THE_GOONIES_OBJECT

class GObject
{
    public:
        GObject(int x, int y, int sfx_volume);
        virtual ~GObject();

        virtual bool cycle(class VirtualController *k, class GMap *map, int layer, class TheGoonies *game, class GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

        int get_layer(void)
        {
            return m_layer;
        };
        void set_layer(int layer, GMap *map);

        float get_x(void)
        {
            return m_x;
        };
        float get_y(void)
        {
            return m_y;
        };
        void set_x(float x)
        {
            m_x = x;
        };
        void set_y(float y)
        {
            m_y = y;
        };

        int get_state(void)
        {
            return m_state;
        };
        void set_state(int state)
        {
            m_state = state;
        };

        virtual bool collision(GObject *o);
        virtual bool collision(int xoffs, int yoffs, GObject *o);
        virtual bool collision(class GLTile *t, int x, int y);
        virtual bool collision(class C2DCMC *t, int x, int y);
        virtual bool collision(int xoffs, int yoffs, GLTile *t, int x, int y);

        Symbol *get_class(void)
        {
            return m_class;
        };


        GLTile *get_last_tile_used(void)
        {
            return m_last_tile_used;
        };

        // distance between the GObject and the player
        int get_distance(GMap *map);

        // 'angle' of this object based on player position
        int get_angle(GMap *map);

    protected:

        int m_sfx_volume;
        int m_state;
        int m_state_cycle;
        float m_x, m_y;
        int m_layer;
        Symbol *m_class;
        GLTile *m_last_tile_used; // used to ease collision detection
};

#endif

