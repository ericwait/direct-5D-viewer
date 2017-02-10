#pragma once
#include "Message.h"
#include "RenderMessages.h"

#include "QueuePolygon.h"

// Control messages
class MessageClose: public Message
{
public:
	MessageClose(){}

protected:
	virtual bool process(){return false;}
};


// Volume setup and load messages
class MessageInitVolume: public Message
{
public:
	MessageInitVolume(int numFrames, int numChannels, Vec<size_t> dims, Vec<float> physSize, bool columnMajor);

protected:
	virtual bool process();

private:
	bool columnMajor;

	int numChannels;
	int numFrames;

	Vec<size_t> dims;
	Vec<float> physicalSize;
};


class MessageLoadTextureFrame: public Message
{
public:
	MessageLoadTextureFrame(GraphicObjectTypes type, int frame, unsigned char* data);

protected:
	virtual bool process();

private:
	GraphicObjectTypes textureType;
	int frame;

	unsigned char* imageData;
};


class MessageLoadTexture: public Message
{
public:
	MessageLoadTexture(GraphicObjectTypes inType, unsigned char* inData);

protected:
	virtual bool process();

private:
	GraphicObjectTypes textureType;

	unsigned char* imageData;
};


// Polygon handling
class MessageLoadPolys : public Message
{
public:
	MessageLoadPolys(size_t numPolys);
	virtual ~MessageLoadPolys();

	void addPoly(QueuePolygon* newPolygon);

protected:
	virtual bool process();

private:
	std::vector<QueuePolygon*> polygons;

	// TODO: Get rid of this!
	static std::vector<SceneNode*> rootNodes;
};


class MessageDeletePoly : public Message
{
public:
	MessageDeletePoly(int index) : index(index){}

protected:
	virtual bool process();

private:
	int index;
};


class MessageDeleteAllPolys : public Message
{
public:
	MessageDeleteAllPolys(){}

protected:
	virtual bool process();
};
