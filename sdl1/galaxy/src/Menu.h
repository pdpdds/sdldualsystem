#ifndef MENU_H_
#define MENU_H_

//Includes
#include 	"Button.h"				//Use of the Screen object
#include 	"SDL.h"				//For SDL access

class Menu {
	
public:
	
	//Attributes
	static const unsigned short NUMBER_BUTTONS = 4;
	Button buttons[NUMBER_BUTTONS];
	
	//Constructors - Destructors
	Menu();
	virtual ~Menu();
	
	//Methods
	int Over(int param_x, int param_y);
	int Changed(int param_x, int param_y);

private:
	
	//Attributes
	int selection;
	
	//Methods
	void Init();

};

#endif /*MENU_H_*/
