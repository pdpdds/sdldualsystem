#include <stdio.h>
#include <android/bitmap.h>
#include <jni.h>

#include <game.h>
#include <gamelib.h>
#include <types.h>

#include "androiddata.h"
#include "require_android.h"


#define _CLASSNAME "com/poweredbytoast/tunneltanks/TunnelTanksNative"

static void handle_touch(JNIEnv *env, jclass classy, jint is_touching) {
	_DATA.c_is_touching = is_touching;
}

static void handle_move(JNIEnv *env, jclass classy, jint x, jint y) {
	_DATA.c_touch = VECTOR(x, y);
}

static void handle_dir(JNIEnv *env, jclass classy, jint x, jint y) {
	_DATA.c_dir = VECTOR(x, y);
}

static void handle_button(JNIEnv *env, jclass classy, jint is_pressing) {
	_DATA.c_button = !!is_pressing;
}

static jint handle_step(JNIEnv *env, jclass classy, jclass bitmap) {
	AndroidBitmapInfo info;
	jint out;
	
	/* Load up the bitmap: */
	if( AndroidBitmap_getInfo(env, bitmap, &info)<0 ) {
		/* Bitmap load failed! */
		gamelib_error("GetInfo() failed on the provided bitmap.");
		return 1; /* Will trigger an exit. */
	}
	
	/* Make sure the bitmap is the correct format: */
	if( info.format != ANDROID_BITMAP_FORMAT_RGB_565 ) {
		gamelib_error("We only understand RGB565 format for now...");
		return 1;
	}
	
	/* Queue up a resize event if it's coming: */
	if(info.width != _DATA.prev.w || info.height != _DATA.prev.h) {
		_DATA.next_event = GAME_EVENT_RESIZE;
		_DATA.prev = RECT(0,0, info.width, info.height);
	}
	
	/*if( info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 ) {
		gamelib_error("We only understand RGBA8888 format for now...");
		return 1;
	}*/
	
	/* Preemptively lock the bitmap's pixels: */
	if(AndroidBitmap_lockPixels(env, bitmap, &_DATA.pixels)) {
		gamelib_error("Failed to lock the bitmap's pixel array.");
		return 1;
	}
	
	/* Store the bitmap info for later: */
	_DATA.bmpW = info.width;
	_DATA.bmpH = info.height;
	_DATA.bmpStride = info.stride;
	
	/* Now, store the JNI info for later... */
	/* TODO: is this used anymore? */
	_DATA.env    = env;
	_DATA.bitmap = bitmap;
	
	/* Need to init? */
	if(!_DATA.was_init) {
		_DATA.was_init = 1;
		gamelib_init();
		gamelib_print("Native code initialized.");
	}
	
	/* Now step the simulation: */
	out = game_step(_DATA.gd);
	
	/* Free up those locked pixels: */
	if(AndroidBitmap_unlockPixels(env, bitmap)) {
		gamelib_error("Failed to unlock bitmap.");
		return 1;
	}
	
	return out;
}


#define _METHOD(name, sig, ptr) ((JNINativeMethod){(name), (sig), (ptr)})

/* When the library is first loaded, connect, and init everything: */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *ignored) {
	JNIEnv *env;
	jclass  classy;
	
	JNINativeMethod list[] = {
		_METHOD("setTouchPos",   "(II)V",                        handle_move),
		_METHOD("setIsTouching", "(I)V",                         handle_touch),
		_METHOD("setDirection",  "(II)V",                        handle_dir),
		_METHOD("setPress",      "(I)V",                         handle_button),
		_METHOD("gameStep",      "(Landroid/graphics/Bitmap;)I", handle_step)
	};
	
	/* If this func was passed 0 as a vm, then we know that this was just a
	 * dummy call to this function, so that the linker won't clobber it: */
	if(!vm) return 0;
	
	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
		return -1;
	
	/* Find our native interface class: */
	classy = (*env)->FindClass(env, _CLASSNAME);
	
	/* Register all of our native functions: */
	(*env)->RegisterNatives(env, classy, list, 5);
	
	return JNI_VERSION_1_6;
}

/* When the libary is garbage collected, free all of our resources: */
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *ignored) {
	if(!vm) return;
	
	if(_DATA.was_init) gamelib_exit();
}

