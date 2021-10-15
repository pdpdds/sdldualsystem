// X_INFO.C
//
// Xargon Info
//
// by Allen W. Pilgrim

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
//#include <io.h>
#include "include/gr.h"
#include "include/windows.h"
#include "include/x_obj.h"
#include "include/xargon.h"

extern char tilefile[];

void init_info (void) {
	int c,d,temp;
	char len;
	int flagsdef=f_notvine|f_notstair|f_notwater;
	FILE* blockfile;

	for (c=0; c<numinfotypes; c++) {
		info [c].sh=0x4500;
		info [c].na="";
		info [c].flags=flagsdef;
		};

	blockfile=fopen (tilefile,"rb");
	while (fread(&c,1, 2, blockfile)>0) {
		fread(&info[c].sh, 1, 2, blockfile);
		fread(&temp, 1, 2, blockfile);
		info[c].flags^=temp;
		fread(&len, 1, 1, blockfile);
		info [c].na=(char*)malloc (len+1);
		fread(info[c].na, 1, len,blockfile);
		info[c].na[len]=0;
		};

	for (c=0; c<numstates; c++) {
		stateinfo [c]=0;
		};

	stateinfo [st_begin]    |= sti_invincible;
	stateinfo [st_stand]    |= sti_canfire;
	stateinfo [st_still]    |= sti_invincible;
	stateinfo [st_jumping]  |= sti_canfire;
	stateinfo [st_climbing] |= 0;
	stateinfo [st_die]      |= sti_invincible;
	stateinfo [st_transport]|= sti_invincible;
	stateinfo [st_platform] |= sti_canfire;
	};
