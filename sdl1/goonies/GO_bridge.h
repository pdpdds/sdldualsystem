#ifndef _THE_GOONIES_OBJECT_BRIDGE
#define _THE_GOONIES_OBJECT_BRIDGE

class GO_bridge : public GObject
{
    public:
        GO_bridge(int x, int y, int sfx_volume, int type);

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);
        void shaking();

    protected:
        int m_type;
		float m_y_offset,m_y_old_offset;
		bool m_someone_over_in_the_last_cycle;	// When this changes from "false" to "true", the bridge makes noise
		bool m_forced_offset;		// "true" if we are in a left or right bridge piece or if a character is over the piece
		float m_y_forced_offset;	// Desired offset when a force is applied (0 for left/right or +4 for character over)
		bool m_searched_for_bridges;
		GO_bridge *m_left_bridge,*m_right_bridge;
};

#endif
