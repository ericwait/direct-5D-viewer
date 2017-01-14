#include "Global/Globals.h"
#include "D3d/MessageProcessor.h"
#include "Global/Vec.h"

class Image
{
public:
	Image(int numChannels, int numFrames, Vec<size_t> dimensions);
	~Image();

	unsigned char* getPixels();
	void setPixels(unsigned char* pixels);

	int getNumChannels();
	void setNumChannels(int numChannels);

	int getNumFrames();
	void setNumFrames(int numFrames);

	Vec<size_t> getDimensions();
	void setDimensions(Vec<size_t> dims);

	Vec<float> getPhysicalDims();
	void setPhysicalDim(Vec<float> pDims);

	void setTextureType(GraphicObjectTypes numArgs);
	GraphicObjectTypes getTextureType();

private:
	Image();
	void setDefauts();

	GraphicObjectTypes textureType;
	unsigned char* pixels;
	Vec<size_t> dimensions;
	int numChannels;
	int numFrames;
	Vec<float> physicalDims;
};