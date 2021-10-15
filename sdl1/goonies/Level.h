#ifndef _GONNIES_LEVEL
#define _GOONIES_LEVEL

class GLevel {
public:
	GLevel();
	~GLevel();

	void setName(char *name);
	void setPassword(char *pwd);
	char *getName(void);
	char *getPassword(void);
	
protected:
	Symbol *m_name;
	Symbol *m_password;

};


#endif
