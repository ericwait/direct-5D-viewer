#pragma once
#include "Message.h"

// Messages that require direct windowing system access (HWND)
class MessageSetWindowSize: public Message
{
public:
	MessageSetWindowSize(int width, int height) : width(width), height(height){}

protected:
	virtual bool process();

private:
	int width;
	int height;
};
