#include <drawbuffer.h>
#include <malloc.h>
#include <memalloc.h>

/* Various colors for use in the game: */
Color color_dirt_hi       = COLOR(0xc3, 0x79, 0x30);
Color color_dirt_lo       = COLOR(0xba, 0x59, 0x04);
Color color_rock          = COLOR(0x9a, 0x9a, 0x9a);
Color color_fire_hot      = COLOR(0xff, 0x34, 0x08);
Color color_fire_cold     = COLOR(0xba, 0x00, 0x00);
Color color_blank         = COLOR(0x00, 0x00, 0x00);
Color color_bg            = COLOR(0x00, 0x00, 0x00);
Color color_bg_dot        = COLOR(0x00, 0x00, 0xb6);
Color color_status_bg     = COLOR(0x65, 0x65, 0x65);
Color color_status_energy = COLOR(0xf5, 0xeb, 0x1a);
Color color_status_health = COLOR(0x26, 0xf4, 0xf2);

/* Primary colors: */
Color color_primary[8] = {
	COLOR(0x00, 0x00, 0x00),
	COLOR(0xff, 0x00, 0x00),
	COLOR(0x00, 0xff, 0x00),
	COLOR(0xff, 0xff, 0x00),
	COLOR(0x00, 0x00, 0xff),
	COLOR(0xff, 0x00, 0xff),
	COLOR(0x00, 0xff, 0xff),
	COLOR(0xff, 0xff, 0xff)
};

Color color_tank[8][3] = {
	/* Blue tank: */
	{ COLOR(0x2c,0x2c,0xff), COLOR(0x00,0x00,0xb6), COLOR(0xf3,0xeb,0x1c) },
	
	/* Green tank: */
	{ COLOR(0x00,0xff,0x00), COLOR(0x00,0xaa,0x00), COLOR(0xf3,0xeb,0x1c) },
	
	/* Red tank: */
	{ COLOR(0xff,0x00,0x00), COLOR(0xaa,0x00,0x00), COLOR(0xf3,0xeb,0x1c) },
	
	/* Pink tank: */
	{ COLOR(0xff,0x99,0x99), COLOR(0xaa,0x44,0x44), COLOR(0xf3,0xeb,0x1c) },
	
	/* Purple tank: */
	{ COLOR(0xff,0x00,0xff), COLOR(0xaa,0x00,0xaa), COLOR(0xf3,0xeb,0x1c) },
	
	/* White tank: */
	{ COLOR(0xee,0xee,0xee), COLOR(0x99,0x99,0x99), COLOR(0xf3,0xeb,0x1c) },
	
	/* Aqua tank: */
	{ COLOR(0x00,0xff,0xff), COLOR(0x00,0xaa,0xaa), COLOR(0xf3,0xeb,0x1c) },
	
	/* Gray tank: */
	{ COLOR(0x66,0x66,0x66), COLOR(0x33,0x33,0x33), COLOR(0xf3,0xeb,0x1c) }
};


/* TODO: We're using color structures here because we started with Uint32 values
 *       and this was an easier transition. Eventually, all colors will be in a
 *       central array, and the pixel data will simply be 1-byte indexes. */
struct DrawBuffer {
	Color *pixel_data;
	unsigned w, h;
	Color default_color;
};


DrawBuffer *drawbuffer_new(unsigned w, unsigned h) {
	DrawBuffer *out = malloc(sizeof(DrawBuffer));
	out->pixel_data = malloc(sizeof(Color) * w * h);
	out->w = w; out->h = h;
	out->default_color.b = 0;
	out->default_color.r = 0;
	out->default_color.g = 0;
	
	return out;
}

void drawbuffer_destroy(DrawBuffer *b) {
	if(!b) return;
	free(b->pixel_data);
	free(b);
}

void drawbuffer_set_default(DrawBuffer *b, Color color) {
	b->default_color = color;
}

void drawbuffer_set_pixel(DrawBuffer *b, unsigned x, unsigned y, Color color) {
	if(x >= b->w || y >= b->h) return;
	b->pixel_data[ y * b->w + x ] = color;
}

Color drawbuffer_get_pixel(DrawBuffer *b, unsigned x, unsigned y) {
	if(x >= b->w || y >= b->h) return b->default_color;
	return b->pixel_data[ y * b->w + x ];
}

