#ifndef _THEGOONIES_GAME
#define _THEGOONIES_GAME

#define ROOM_TRANSITION_TIME 15

extern int hiscore;

class GMapHolder
{
    public:
        ~GMapHolder()
        {
            delete m_map;
            m_map = 0;
        }

        class GMap *m_map;
        int m_l;
        int m_x;
        int m_y;
};

class TheGoonies
{
    public:
        TheGoonies(int level, int roomx, int roomy, int sfx_volume,
                   int music_volume, int ambient_volume, GLTManager *GLTM);
        ~TheGoonies();

        void clear(void);
        void level_change(int level, int roomx, int roomy, GLTManager *GLTM);

        bool cycle(class VirtualController *k, GLTManager *GLTM, SFXManager *SFXM, MusicManager *MusicM);
        void draw(GLTManager *GLTM);

        void set_music_volume(int v, MusicManager *MusicM);
        void set_sfx_volume(int v);
        void set_ambient_volume(int v, MusicManager *MusicM);

        int get_hiscore(void)
        {
            return hiscore;
        };
        void set_hiscore(int s)
        {
            hiscore = s;
        };

        bool levelCompleteP(void)
        {
            return m_level_complete;
        };
        class GO_character *get_character(void)
        {
                return m_character;
        };

        void add_multiroom_object(GObject *o, GMapHolder *map);
        void add_multiroom_object(GObject *o, GMap *map);
        void delete_multiroom_object(GObject *o);

		bool object_exists(GObject *o);

		bool game_paused();

    protected:

        void map_filename(char *fn, int level, int roomx, int roomy);
        GMapHolder *get_map(int l, int x, int y);    // if a new map is loaded, it is placed in m_maps
        GMapHolder *get_map_not_player(int l, int x, int y); // if a new map is loaded, it is placed in m_maps_buffer
        GMapHolder *get_map_noload(int l, int x, int y);

        List<GMapHolder> m_maps, m_maps_buffer; /* - m_maps contains the maps the player has been in.
                      - m_buffer contains other maps that have been loaded, but that the player
                        still has not visited (may be they have been loaded to execute the code
                        of a fratelli gang).
                   */
        GMapHolder *m_current_map;
        class GO_character *m_character;

        // Variables for the room transition effect:
        GMapHolder *m_previous_map;
        int m_previous_map_direction; // 0 left, 1 right, 2 up, 3 down
        int m_previous_map_cycle;
		
        bool m_level_complete;
        bool m_player_critical;

        // Multi-room objects (fratellies, doubloons, ghosts):
        List<GObject> m_multiroom_objects;
        List<GMapHolder> m_multiroom_objects_map;
        List<GObject> m_multiroom_objects_todelete;

        int m_current_zoom; // = 0 if normal zoom, and goes to 50 till maximum zoom
        int m_cycle;

        // volumes:
        int m_sfx_volume, m_music_volume, m_ambient_volume;

        // pause/map variables:
        int m_gamepaused;
        int m_gamepaused_timmer;
        bool m_pausemap_r_computed;
        int m_pausemap_rx1, m_pausemap_rx2, m_pausemap_ry1, m_pausemap_ry2;

        // table that contains which music to play at each screen:
        List<char> m_music_tbl_room, m_music_tbl_music, m_music_tbl_ambient;
        char m_current_music[80];
        char m_current_ambient[80];
};

#endif

