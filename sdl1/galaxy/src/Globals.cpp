#include	"Globals.h"					//Header file

string IntegerToString(int integer) {
	stringstream sstr;
	sstr << integer;
	return sstr.str();
}
