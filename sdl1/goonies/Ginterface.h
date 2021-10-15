#ifndef G_INTERFACE
#define G_INTERFACE

class G_InterfaceElement {
public:
	G_InterfaceElement();
	virtual ~G_InterfaceElement();

	virtual bool check_status(int mousex,int mousey,int button,int button_status,class KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	int m_ID;
	bool m_modal;	/* If any element is modal, only him has the control until it is destroyed (the rest of the interface is faded) */
	bool m_enabled;
	bool m_active;	/* This indictes wether the component is active or passive, */
					/* e.g.: G_Text and G_Frame are passive					*/
	bool m_to_be_deleted;

	float m_x,m_y,m_dx,m_dy;
};


class G_Text : public G_InterfaceElement {
public:

	G_Text(char *text,float x,float y,bool centered);
	virtual ~G_Text();

	virtual void draw(float alpha);
	virtual void draw(void);

	bool m_centered;
	char *m_text;
};


class G_Button : public G_InterfaceElement {
public:

	G_Button(char *text,float x,float y,float dx,float dy,int ID,class GLTManager *GLTM);
	G_Button(GLTile *icon,float x,float y,float dx,float dy,int ID,GLTManager *GLTM);
	virtual ~G_Button();

	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	char *m_text;
	int m_status;
	GLTile *m_tile;
	GLTManager *m_GLTM;
	int m_width,m_height;
};


class G_SButton : public G_Button {
public:

	G_SButton(char *text,float x,float y,float dx,float dy,int ID,class GLTManager *GLTM);
	G_SButton(GLTile *icon,float x,float y,float dx,float dy,int ID,GLTManager *GLTM);
	virtual ~G_SButton();

	virtual void draw(float alpha);
};


class G_ButtonTransparent : public G_Button {
public:

	G_ButtonTransparent(char *text,float x,float y,float dx,float dy,int ID);

	virtual void draw(float alpha);
};



class G_Browser : public G_InterfaceElement {
public:

	G_Browser(float x,float y,float dx,float dy,int ID);
	virtual ~G_Browser();

	void clear(void);
	void addEntry(char *);
	char *getEntry(int i);
	void deleteEntry(int i);
	void setSelected(int i);
	int getSelected(void);

	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	List<char> m_entries;
	float m_slider_pos,m_slider_height;
	int m_selected,m_mouse_over;
	int m_old_mouse_x,m_old_mouse_y;

};


class G_TextInput : public G_InterfaceElement {
public:

	G_TextInput(char *initial_text,char *message,int max_characters,float x,float y,float dx,float dy,int ID,GLTManager *GLTM);
	virtual ~G_TextInput();

	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	GLTManager *m_GLTM;
	float m_max_dx;
	int m_width,m_height;
	char *m_editing;
	char *m_message;
	int m_max_length;
	unsigned int m_editing_position;
	int m_state,m_cycle;
};


class G_Message : public G_InterfaceElement {
public:

	G_Message(char *message,float x,float y,int ID,GLTManager *GLTM);
	virtual ~G_Message();

	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	GLTManager *m_GLTM;
	char *m_message;
	int m_state,m_cycle;
	G_SButton *m_ok_button;
};


class G_Confirmation : public G_InterfaceElement {
public:

	G_Confirmation(char *message,float x,float y,int ID,GLTManager *GLTM);
	virtual ~G_Confirmation();

	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	GLTManager *m_GLTM;
	char *m_message;
	int m_state,m_cycle;
	G_SButton *m_ok_button,*m_cancel_button;
};


class G_interface {
public:
	static List<G_InterfaceElement> m_elements;

	static void add_element(G_InterfaceElement *b);
	static void remove_element(G_InterfaceElement *b);
	static void substitute_element(G_InterfaceElement *old,G_InterfaceElement *n);
	static void reset(void);
	static int update_state(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	static void draw(float alpha);
	static void draw(void);

	static G_InterfaceElement *get(int ID);
	static void enable(int ID);
	static void disable(int ID);
};

#endif

