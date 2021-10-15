#ifndef _GLTILE_MANAGER
#define _GLTILE_MANAGER


class GLTManagerNode
{
    public:
		~GLTManagerNode();

        GLTile *m_tile;
        Symbol *m_name;
};


class GLTManager
{
    public:
        GLTManager();
        ~GLTManager();

        GLTile *get(char *name);
        GLTile *get(Symbol *name);

        GLTile *get_smooth(char *name);
        GLTile *get_smooth(Symbol *name);

		void cache(void);

    protected:

		char *remove_extension(char *filename);

		List<GLTManagerNode> *m_hash;
        List<GLTManagerNode> *m_hash_smooth;

};

#endif



