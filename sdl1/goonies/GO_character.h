#ifndef _THE_GOONIES_OBJECT_CHARACTER
#define _THE_GOONIES_OBJECT_CHARACTER

#define CSTATE_STANDING_LEFT  0
#define CSTATE_STANDING_RIGHT  1
#define CSTATE_WALKING_LEFT   2
#define CSTATE_WALKING_RIGHT  3
#define CSTATE_JUMPING_LEFT   4
#define CSTATE_JUMPING_RIGHT  5
#define CSTATE_JUMPING_LEFT_LEFT 6
#define CSTATE_JUMPING_RIGHT_RIGHT 7
#define CSTATE_CLIMBING_UP   8
#define CSTATE_CLIMBING    9
#define CSTATE_CLIMBING_DOWN  10
#define CSTATE_FALLING_LEFT   11
#define CSTATE_FALLING_RIGHT  12
#define CSTATE_PUNCH_LEFT   13
#define CSTATE_PUNCH_RIGHT   14
#define CSTATE_JUMPPUNCH_LEFT  15
#define CSTATE_JUMPPUNCH_RIGHT  16
#define CSTATE_JUMPPUNCH_LEFT_LEFT  17
#define CSTATE_JUMPPUNCH_RIGHT_RIGHT 18
#define CSTATE_ENTERING_DOOR  19
#define CSTATE_DYING    20
#define CSTATE_DEAD     21

#define CSTATE_SHOOT_LEFT   13 // only for Fratelli
#define CSTATE_SHOOT_RIGHT   14 // only for Fratelli
#define CSTATE_TAKENDOWN   20 // only for Fratelli

extern int score;

class GO_character : public GObject
{
        friend class TheGoonies;
    public:
        GO_character(int x, int y, int sfx_volume, int facing);
        virtual ~GO_character();

        virtual bool cycle(VirtualController *k, GMap *map, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        virtual void draw(GLTManager *GLTM);

        virtual bool is_a(Symbol *c);
        virtual bool is_a(char *c);

        bool room_change_request(void)
        {
            return m_requested_room;
        };
        void clear_change_request(void)
        {
            m_requested_room = false;
        };

        int get_requested_room_x(void)
        {
            return m_requested_room_x;
        };
        int get_requested_room_y(void)
        {
            return m_requested_room_y;
        };
        int get_requested_room_door(void)
        {
            return m_requested_room_door;
        };

        bool has_key(void)
        {
            return m_key;
        };
        void use_key(void)
        {
            m_key = false;
        };

        void set_rescued_goonies(int n)
        {
            m_goonies_rescued = n;
        };
        int get_rescued_goonies(void)
        {
            return m_goonies_rescued;
        };

		void set_goonie_is_rescued(int n, bool r)
		{
			m_rescued_goonies[n] = r;
		}
		bool get_goonie_is_rescued(int n)
		{
			return m_rescued_goonies[n];
		}

        int get_energy(void)
        {
            return m_energy;
        };
        int get_experience(void)
        {
            return m_experience;
        };
        int get_score(void)
        {
            return score;
        };
        void inc_score(int m_score);

        int get_player_angle();
        int get_player_distance()
        {
            return 1;
        };
        int get_hammer_status(void)
        {
            return m_hammer_status;
        };
        void set_hammer_status(int hs)
        {
            m_hammer_status = hs;
        };

        int get_last_state(void)
        {
            return m_last_state;
        };
        Symbol *get_last_pick(void)
        {
            return m_last_pick;
        };
        Symbol *get_last_hit(void)
        {
            return m_last_hit;
        };
        Symbol *get_last_hit_by(void)
        {
            return m_last_hit_by;
        };
        Symbol *get_camefrom(void)
        {
            return m_camefrom;
        };
        void set_camefrom(char *str)
        {
            if (m_camefrom != 0)
                delete m_camefrom;
            m_camefrom = new Symbol(str);
        }
        Symbol *player_has(char *str)
        {
            Symbol *s = new Symbol(str);
            Symbol *r = m_items.MemberGet(s);
            delete s;
            return r;
        };
        void substitute_item(char *str, char *str2)
        {
            Symbol *s = player_has(str);
            if (s != 0) {
                m_items.DeleteElement(s);
                delete s;
                s = new Symbol(str2);
                m_items.Add(s);
            } // if
        };
        void remove_item(char *str)
        {
            Symbol *s = player_has(str);
            if (s != 0) {
                m_items.DeleteElement(s);
                delete s;
            } // if
        };

        int get_bluebadbook_nghosts(void)
        {
            return m_bluebadbook_nghosts;
        };
        void set_bluebadbook_nghosts(int n)
        {
            m_bluebadbook_nghosts = n;
        };

        int get_x(void)
        {
            return int(m_x);
        };
        int get_y(void)
        {
            return int(m_y);
        };
    protected:
        int m_walking_channel;
        int m_climbing_channel;

        int m_punch_cycle;
		int m_step_cycle;

        bool m_requested_room;
        int m_requested_room_x, m_requested_room_y, m_requested_room_door;
        int m_player_hit_counter;
        int m_facebefore_vine;

        float m_walking_speed;

        int m_energy;
        int m_experience;
        bool m_key;
        int m_goonies_rescued;
		bool m_rescued_goonies[7];
        int m_score;

        int m_turning_counter;
        int m_previous_state;

        // variables for scripts conditions:
        int m_last_state;
        Symbol *m_camefrom;
        Symbol *m_last_pick;
        Symbol *m_last_hit, *m_last_hit_by;
        List<Symbol> m_items;

        // object variables:
        int m_yellowhelmet_status, m_yellowhelmet_timer;
        int m_greycoat_status, m_greycoat_timer;
        int m_yellowcoat_timer;
        int m_hammer_status;
        int m_greenbook_status;
        int m_redbook_status;
        int m_lightbluebook_status;
        int m_bluebook_status, m_bluebook_timer;
        int m_greencoat_timer;
        int m_whitebook_status;
        int m_yellowshield_status, m_yellowshield_timer;
        int m_lightbluecoat_timer;
        int m_whiteshield_status, m_whiteshield_timer;
        int m_lightbluehelmet_status, m_lightbluehelmet_timer;
        int m_yellowbook_status, m_yellowbook_timer;
        int m_purpleshield_status, m_purpleshield_timer;
        int m_clock_timer;
        int m_bluebadbook_nghosts;
};

#endif

