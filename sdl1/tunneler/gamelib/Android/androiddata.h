#ifndef _ANDROID_DATA_H_
#define _ANDROID_DATA_H_

#include <jni.h>

#include <gamelib.h>
#include <game.h>
#include <types.h>

typedef struct AndroidData {
	/* The last size of the Bitmap, so we know if it's changed: */
	Rect prev;
	
	/* Store an event: */
	EventType next_event;
	
	/* Four controller variables: */
	Vector c_touch, c_dir;
	unsigned c_button, c_is_touching;
	
	/* We store the current java environment + bitmap jobject here: */
	JNIEnv *env;
	jobject bitmap;
	
	/* The game context variable: */
	GameData *gd;
	
	/* Whether this was initialized: */
	unsigned was_init;
	
	/* Used to cache bitmap information: */
	unsigned bmpW, bmpH, bmpStride;
	void *pixels;
	
} AndroidData;

extern AndroidData _DATA;

#endif /* _ANDROID_DATA_H_ */

