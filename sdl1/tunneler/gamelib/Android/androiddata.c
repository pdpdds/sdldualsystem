#include <stdlib.h>
#include <types.h>
#include <gamelib.h>
#include "androiddata.h"

#include "require_android.h"

AndroidData _DATA = {
	.prev          = RECT(0,0,0,0),
	.next_event    = GAME_EVENT_NONE,
	.c_touch       = VECTOR(0,0),
	.c_dir         = VECTOR(0,0),
	.c_button      = 0,
	.c_is_touching = 0,
	.env           = NULL,
	.bitmap        = NULL,
	.gd            = NULL,
	.was_init      = 0,
	.pixels        = NULL
};

