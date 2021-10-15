/*
 * Bermuda Syndrome engine rewrite
 * Copyright (C) 2007-2011 Gregory Montoir
 */

#ifndef SYSTEMSTUB_H__
#define SYSTEMSTUB_H__

#include "intern.h"

struct Mixer;

struct PlayerInput {
	enum {
		DIR_UP    = 1 << 0,
		DIR_DOWN  = 1 << 1,
		DIR_LEFT  = 1 << 2,
		DIR_RIGHT = 1 << 3
	};
	uint8_t dirMask;
	bool enter;
	bool space;
	bool shift;
	bool ctrl;
	bool tab;
	bool escape;
	bool leftMouseButton, rightMouseButton;
	int mouseX, mouseY;
	bool save;
	bool load;
	int stateSlot;
	bool fastMode;
};

struct SystemStub {
	typedef void (*AudioCallback)(void *param, uint8_t *buf, int len);

	bool _quit;
	PlayerInput _pi;

	virtual ~SystemStub() {}

	virtual void init(const char *title, int w, int h) = 0;
	virtual void destroy() = 0;

	virtual void setIcon(const uint8_t *data, int size) = 0;
	virtual void showCursor(bool show) = 0;

	virtual void setPalette(const uint8_t *pal, int n) = 0;
	virtual void fillRect(int x, int y, int w, int h, uint8_t color) = 0;
	virtual void copyRect(int x, int y, int w, int h, const uint8_t *buf, int pitch, bool transparent = false) = 0;
	virtual void darkenRect(int x, int y, int w, int h) = 0;
	virtual void updateScreen() = 0;

	virtual void setYUV(bool flag, int w, int h) = 0;
	virtual uint8_t *lockYUV(int *pitch) = 0;
	virtual void unlockYUV() = 0;

	virtual void processEvents() = 0;
	virtual void sleep(int duration) = 0;
	virtual uint32_t getTimeStamp() = 0;

	virtual void lockAudio() = 0;
	virtual void unlockAudio() = 0;
	virtual void startAudio(AudioCallback callback, void *param) = 0;
	virtual void stopAudio() = 0;
	virtual int getOutputSampleRate() = 0;

	virtual Mixer *getMixer() = 0;
};

extern SystemStub *SystemStub_SDL_create();

#endif // SYSTEMSTUB_H__
