#ifndef _GONNIES_LEVELPACK
#define _GOONIES_LEVELPACK

class GLevelPack {
public:
	GLevelPack();
	~GLevelPack();

	void addNewLevel(GLevel *level);
	void deleteLevel(GLevel *level);
	int getNLevels(void);
	int getLevelPosition(GLevel *level);
	GLevel *getLevel(int n);
	GLevel *getLevel(char *name);
	void setLevel(int n,GLevel *level);
	
protected:

	List<GLevel> m_levels;
};


#endif
