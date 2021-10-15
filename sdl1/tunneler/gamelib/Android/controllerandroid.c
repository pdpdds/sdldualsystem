#include <stdlib.h>
#include <gamelib.h>
#include <tank.h>
#include "androiddata.h"
#include "require_android.h"

static void android_controller(PublicTankInfo *i, void *d, int *vx, int *vy, unsigned *s) {
	*vx = _DATA.c_dir.x;
	*vy = _DATA.c_dir.y;
	*s  = _DATA.c_button;
}

void controller_android_attach( Tank *t ) {
	tank_set_controller(t, android_controller, NULL);
}

