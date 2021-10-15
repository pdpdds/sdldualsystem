#ifndef __GONNIES_MAP
#define __GONNIES_MAP


class GMapTilePlace
{
    public:
		~GMapTilePlace();
	
        class Symbol *m_tile_name;
		class GLTile *m_tile_cache;
        int m_x, m_y;
};


class GMapCMCPlace
{
    public:
		~GMapCMCPlace();

        class C2DCMC *m_cmc;
        int m_x, m_y;
};

class GObjectPlace
{
    public:
		~GObjectPlace();

        Symbol *m_object_name;
        int m_x, m_y;
        List<int> m_parameters;
};



class GMapLayer
{
        friend class GMap;
    public:
        GMapLayer();
        GMapLayer(FILE *fp);
        ~GMapLayer();

        void reset(int sfx_volume);
        void reset_first(int sfx_volume, GMap *m);
        void cycle(class VirtualController *v, GMap *m, int layer, class TheGoonies *game, class GLTManager *GLTM, class SFXManager *SFXM);
        void cycle_clock(class VirtualController *v, GMap *m, int layer, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        void draw(GLTManager *GLTM);

        void add_object(class GObject *o);
        void add_auxiliar_object(GObject *o);
        bool remove_object(GObject *o);
		bool object_exists(GObject *o);

        void add_tile(char *name, int x, int y);

        bool collision(GObject *o, GLTManager *GLTM);
        bool collision_with_background(GObject *o, GLTManager *GLTM);
        bool collision_with_background(GObject *o, int x_offs, int y_offs, GLTManager *GLTM);
        bool collision_with_background(GLTile *t, int x, int y, GLTManager *GLTM);
        bool collision_with_background_wo_bigrocks(GObject *o, int x_offs, int y_offs, GLTManager *GLTM);
        GObject *collision_with_object(GObject *o, GLTManager *GLTM);
        GObject *collision_with_object(GObject *o, GLTManager *GLTM, char *type);
        GObject *collision_with_object(GObject *o, GLTManager *GLTM, Symbol *type);
        GObject *collision_with_object(GObject *o, int x_offs, int y_offs, GLTManager *GLTM, char *type);
        GObject *collision_with_object(GObject *o, int x_offs, int y_offs, GLTManager *GLTM, Symbol *type);
        GObject *collision_with_object(GLTile *t, int x, int y, GLTManager *GLTM, char *type);
        GObject *collision_with_object(GLTile *t, int x, int y, GLTManager *GLTM, Symbol *type);

        GObject *get_object(char *type);
        GObject *get_object(Symbol *type);

        void get_objects(List<GObject> *l, char *type);
        void get_objects(List<GObject> *l, Symbol *type);

    private:
        // specification loaded from a file:
        List<GMapTilePlace> m_tiles;
        List<GObjectPlace> m_objects;

        // objects currently running:
        List<GObject> m_running_objects;
        List<GObject> m_auxiliar_objects;
};


class GMap
{
        friend class TheGoonies;
    public:
        GMap(FILE *fp);
        ~GMap();

        void reset(int sfx_volume);
        void reset_first(int sfx_volume);
        void cycle(VirtualController *k, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        void cycle_clock(VirtualController *k, class TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM);
        void draw(GLTManager *GLTM, bool water);
        void draw(int layer, GLTManager *GLTM);
        void draw_darkened_area(int player_x, int player_y);

        void search_for_bridges(GLTManager *GLTM);

        void add_object(GObject *o, int layer);
        void add_auxiliar_object(GObject *o, int layer);
        bool remove_object(GObject *o, int layer);
		bool object_exists(GObject *o);

        int get_dx(void)
        {
            return m_dx;
        };
        int get_dy(void)
        {
            return m_dy;
        };
        int get_n_layers(void)
        {
            return m_layers.Length();
        };

        GMapLayer *get_layer(int n)
        {
            return m_layers[n];
        };

        bool collision(GObject *o, GLTManager *GLTM);
        bool collision_with_background(GObject *o, GLTManager *GLTM);
        bool collision_with_background(GObject *o, int x_offs, int y_offs, GLTManager *GLTM);
        bool collision_with_background(GLTile *t, int x, int y, GLTManager *GLTM);
        bool collision_with_background_wo_bridges(GObject *o, GLTManager *GLTM);
        bool collision_with_background_wo_bridges(GObject *o, int x_offs, int y_offs, GLTManager *GLTM);
        bool collision_with_background_wo_bridges_bigrocks(GObject *o, GLTManager *GLTM);
        bool collision_with_background_wo_bridges_bigrocks(GObject *o, int x_offs, int y_offs, GLTManager *GLTM);
        GObject *collision_with_object(GObject *o, GLTManager *GLTM);
        GObject *collision_with_object(GObject *o, GLTManager *GLTM, char *type);
        GObject *collision_with_object(GObject *o, GLTManager *GLTM, Symbol *type);
        GObject *collision_with_object(GObject *o, int x_offs, int y_offs, GLTManager *GLTM, char *type);
        GObject *collision_with_object(GObject *o, int x_offs, int y_offs, GLTManager *GLTM, Symbol *type);
        GObject *collision_with_object(GLTile *t, int x, int y, GLTManager *GLTM, char *type);
        GObject *collision_with_object(GLTile *t, int x, int y, GLTManager *GLTM, Symbol *type);

        GObject *get_object(char *type);
        GObject *get_object(Symbol *type);

        List<GObject> *get_objects(char *type);
        List<GObject> *get_objects(Symbol *type);

        void get_water_info(void);
        GLuint get_water_info_texture(void);
        void clear_water_info(void);
        int get_water_level(void);
        int get_lava_level(void);

        void set_shake(void)
        {
            m_shake_timmer = 50;
        };

    private:
        int m_dx, m_dy;
        List<GMapLayer> m_layers;
        List<GMapCMCPlace> m_extra_collisions;

        class GooniesScript *m_script;

        bool m_searched_for_bridges;

        // Screen shake:
        float m_shake_timmer;

        // Info for the water effect:
        bool m_water_info_captured;
        int m_water_level, m_lava_level;
        GLuint m_water_info_texture;


};

#endif

