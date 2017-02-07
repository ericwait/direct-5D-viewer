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


// Texture Loading
class MessageLoadTexture: public Message
{
public:
	MessageLoadTexture(int numChannels, int numFrames, Vec<size_t> dims, unsigned char* inData);

	void setTextureType(GraphicObjectTypes inType);
	void setPhysSize(Vec<float> physSize);

protected:
	virtual bool process();

private:
	GraphicObjectTypes textureType;

	bool columnMajor;

	int numChannels;
	int numFrames;

	Vec<size_t> dims;
	Vec<float> physicalSize;

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
