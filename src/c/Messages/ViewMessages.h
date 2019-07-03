#pragma once
#include "Message.h"
#include "RenderMessages.h"

#include "D3d/Renderer.h"

#include <set>

// Basic view messages
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

class MessageSetDpiScale : public Message
{
public:

	MessageSetDpiScale(int scale) : scale(scale) {}

protected:
	virtual bool process();

private:
	int scale;
};


class MessageResetView: public Message
{
public:
	MessageResetView(){};

protected:
	virtual bool process();
};


class MessageSetViewOrigin: public Message
{
public:
	MessageSetViewOrigin(Vec<float> origin) : origin(origin){}

protected:
	virtual bool process();

private:
	Vec<float> origin;
};


class MessageSetViewRotation: public Message
{
public:
	MessageSetViewRotation(Vec<double> axis, double angle) : axis(axis), angle(angle){}

protected:
	virtual bool process();

private:
	Vec<double> axis;
	double angle;
};


class MessageSetWorldRotation: public Message
{
public:
	MessageSetWorldRotation(Vec<double> axis, double angle) : axis(axis), angle(angle){}

protected:
	virtual bool process();

private:
	Vec<double> axis;
	double angle;
};


class MessageSetFrontClipPlane: public Message
{
public:
	MessageSetFrontClipPlane(float clipDistance) : clipDistance(clipDistance){}

protected:
	virtual bool process();

private:
	float clipDistance;
};


// Show/Hide GUI elements
class MessageShowFrame: public Message
{
public:
	MessageShowFrame(bool visible) : visible(visible){}

protected:
	virtual bool process();

private:
	bool visible;
};


class MessageShowLabels: public Message
{
public:
	MessageShowLabels(bool visible) : visible(visible){}

protected:
	virtual bool process();

private:
	bool visible;
};


class MessageShowScale: public Message
{
public:
	MessageShowScale(bool visible) : visible(visible){}

protected:
	virtual bool process();

private:
	bool visible;
};


// Color settings
class MessageSetBackgroundColor: public Message
{
public:
	MessageSetBackgroundColor(Vec<float> color): color(color) {}

protected:
	virtual bool process();

private:
	Vec<float> color;
};


class MessageSetObjectColor: public Message
{
public:
	MessageSetObjectColor(GraphicObjectTypes type, Vec<float> color) : type(type), color(color){}

protected:
	virtual bool process();

private:
	GraphicObjectTypes type;
	Vec<float> color;
};


// Camera move messages
class MessageMoveCamera: public Message
{
public:
	MessageMoveCamera(Vec<double> delta) : delta(Vec<float>(delta)){}

protected:
	virtual bool process();

private:
	Vec<float> delta;
};


// Graphics visibility selection
class MessageShowPolys: public Message
{
public:
	MessageShowPolys(bool visible): visible(visible) {}

	void setPoly(int index);

protected:
	virtual bool process();

private:
	bool visible;

	std::set<int> indices;
};


class MessageShowObjectType: public Message
{
public:
	MessageShowObjectType(GraphicObjectTypes type, bool visible) : type(type), visible(visible){}

protected:
	virtual bool process();

private:
	GraphicObjectTypes type;
	bool visible;
};


// Polygon visualization settings
class MessageSetPolyWireframe: public Message
{
public:
	MessageSetPolyWireframe(bool wireframe) : wireframe(wireframe){}

protected:
	virtual bool process();

private:
	bool wireframe;
};


class MessageSetPolyLighting: public Message
{
public:
	MessageSetPolyLighting(bool lightingOn) : lightingOn(lightingOn){}

protected:
	virtual bool process();

private:
	bool lightingOn;
};


// Texture visualization messages
class MessageUpdateTransferFcn: public Message
{
public:
	MessageUpdateTransferFcn(GraphicObjectTypes type, int channel) : type(type), channel(channel){}

	void setTransferFcn(Vec<float> transferFcnIn);
	void setRange(Vec<float> rangeIn);
	void setColor(Vec<float> colorIn, float alphaIn);

protected:
	virtual bool process();

private:
	GraphicObjectTypes type;
	int channel;

	Vec<float> transferFcn;
	Vec<float> range;

	Vec<float> color;
	float alpha;
};


class MessageSetTextureLighting: public Message
{
public:
	MessageSetTextureLighting(GraphicObjectTypes type, bool lightingOn) : type(type), lightingOn(lightingOn){};

protected:
	virtual bool process();

private:
	GraphicObjectTypes type;
	bool lightingOn;
};


class MessageSetTextureAttenuation: public Message
{
public:
	MessageSetTextureAttenuation(GraphicObjectTypes type, bool attenuate) : type(type), attenuate(attenuate){};

protected:
	virtual bool process();

private:
	GraphicObjectTypes type;
	bool attenuate;
};
