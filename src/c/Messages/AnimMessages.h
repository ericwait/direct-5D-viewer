#pragma once
#include "Message.h"
#include "RenderMessages.h"

#include "Global/Globals.h"

#include <string>

// TODO: Local message handling in the directx window could still cause a render, is this a problem?
// Let mex take/release control of the render loop
class MessageTakeControl: public Message
{
public:
	MessageTakeControl(){}

protected:
	virtual bool process()
	{
		gRenderer->setRendering(false);
		return true;
	}
};


class MessageReleaseControl: public Message
{
public:
	MessageReleaseControl(){}

protected:
	virtual bool process()
	{
		gRenderer->setRendering(true);
		gRenderer->forceUpdate();

		return true;
	}
};


// Movie play settings
class MessageSetPlayMovie: public Message
{
public:
	MessageSetPlayMovie(bool playing) : playing(playing){}

protected:
	virtual bool process()
	{
		gPlay = playing;
		return true;
	}

private:
	bool playing;
};


class MessageSetSpinning: public Message
{
public:
	MessageSetSpinning(bool spinning) : spinning(spinning){}

protected:
	virtual bool process()
	{
		gRotate = spinning;
		return true;
	}

private:
	bool spinning;
};


class MessageSetMovieFrame: public Message
{
public:
	MessageSetMovieFrame(int frame) : frame(frame){}

protected:
	virtual bool process();

private:
	int frame;
};


// Movie capture settings
class MessageSetCapturePath: public Message
{
public:
	MessageSetCapturePath(const std::string& path, const std::string& filename)
		: path(path), filename(filename)
	{}

protected:
	virtual bool process();

private:
	std::string path;
	std::string filename;
};


class MessageCaptureWindow: public Message
{
public:
	struct BMPData
	{
		int width;
		int height;

		unsigned char* data;
	};

	MessageCaptureWindow(BMPData* outData = NULL) : outData(outData){}

protected:
	virtual bool process();

private:
	BMPData* outData;
};


class MessageCaptureSpinMovie: public Message
{
public:
	MessageCaptureSpinMovie(){};

protected:
	virtual bool process();
};
