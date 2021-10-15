#ifndef _THE_GOONIES_OBJECT_SKULLDOOR
#define _THE_GOONIES_OBJECT_SKULLDOOR

class GO_skulldoor : public GObject
{
    public:
        GO_skulldoor(int x, int y, int sfx_volume);
        GO_skulldoor(int x, int y, int sfx_volume, int dx, int dy, int dd);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

        int get_destination_x(void)
        {
            return m_destination_x;
        };
        int get_destination_y(void)
        {
            return m_destination_y;
        };
        int get_destination_door(void)
        {
            return m_destination_door;
        };

    protected:
        int m_destination_x, m_destination_y, m_destination_door;
		int m_opening_counter;
		int m_glow_counter;
};

#endif

