//Includes
#include 	"Menu.h"				//Own funtions definition

Menu::Menu() {
	//Init every button
	Init();
}

Menu::~Menu() {
}

//Initialize every component of the menu
void Menu::Init() {

	//Init the buttons of the menu
	selection = 0;							//Init the selection

	//Specific configuration
	int x = 374;
	int y = 284;

	//First button, PLAY
	buttons[0].x = x;						//Real screen coordenates
	buttons[0].y = y;

	//First button, SCORE
	buttons[1].x = x;						//Real screen coordenates
	buttons[1].y = (y + 100);

	//First button, CREDITS
	buttons[2].x = x;						//Real screen coordenates
	buttons[2].y = (y + 200);

	//First button, EXIT
	buttons[3].x = x;						//Real screen coordenates
	buttons[3].y = (y + 300);

	//General configuration
	int h = 46;
	int w = 276;
	for(int n = 0; n < NUMBER_BUTTONS; n++) {
		buttons[n].image = new SDL_Rect();		//Image in sprite image
		buttons[n].image->x = 0;
		buttons[n].image->y = n * h;
		buttons[n].image->h = h;
		buttons[n].image->w = w;
		buttons[n].image_over = new SDL_Rect();	//Image over in sprite image
		buttons[n].image_over->x = w;
		buttons[n].image_over->y = n * h;
		buttons[n].image_over->h = h;
		buttons[n].image_over->w = w;
	}
}

//Returns the index of the selected button, -1 if no button is selected
int Menu::Over(int param_x, int param_y) {

	//Assumes that no button is selected
    int button_selected = -1;

    for(int n = 0; n < NUMBER_BUTTONS; n++) {
		if(buttons[n].Over(param_x, param_y)) {
			button_selected = n;
			n = NUMBER_BUTTONS;	//To breack the cycle.
		}
    }

	return button_selected;
}

//Verifies if the selection of the menu has changed.
//Return -1 if the selection hasn't change, otherwise return the selection index.
//Note: 0 = no button selected.
int Menu::Changed(int param_x, int param_y) {

    //Backup the old selection
    int old_selection = selection;

    //Assumes that no button is selected
    selection = 0;
    for(int n = 0; n < NUMBER_BUTTONS; n++) {
		if(buttons[n].Over(param_x, param_y)) {
			selection = (n + 1) ;
			n = NUMBER_BUTTONS;	//To breack the cycle.
		}
    }

    if(old_selection == selection) {			//The selection hasn't change.
    	return -1;
    } else {
    	return selection;
    }
}

