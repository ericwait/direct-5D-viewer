#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"
#include "Image.h"

Image::Image(int numChannels,int numFrames,Vec<size_t> dimensions)
{
	setDefauts();

	this->dimensions = dimensions;
	this->numChannels = numChannels;
	this->numFrames = numFrames;

	size_t pixelsCount = dimensions.product() * numFrames * numChannels;
	pixels = new unsigned char[pixelsCount];
}

void Image::setDefauts()
{
	textureType = GraphicObjectTypes::OriginalVolume;
	dimensions = Vec<size_t>(0,0,0);
	numChannels = 0;
	numFrames = 0;
	physicalDims = Vec<float>(0.0f,0.0f,0.0f);
}

Image::~Image()
{
	setDefauts();
	delete[] pixels;
}

GraphicObjectTypes Image::getTextureType()
{
	return textureType;
}
void Image::setTextureType(GraphicObjectTypes numArgs)
{
	this->textureType = numArgs;
}

unsigned char* Image::getPixels()
{
	return pixels;
}
void Image::setPixels(unsigned char* pixels)
{
	memcpy(this->pixels,pixels,dimensions.product()* numChannels * numFrames * sizeof(unsigned char));
}

int Image::getNumChannels()
{
	return numChannels;
}
void Image::setNumChannels(int numChannels)
{
	this->numChannels = numChannels;
}

int Image::getNumFrames()
{
	return numFrames;
}
void Image::setNumFrames(int numFrames)
{
	this->numFrames = numFrames;
}

Vec<size_t> Image::getDimensions()
{
	return dimensions;
}
void Image::setDimensions(Vec<size_t> dimensions)
{
	this->dimensions = dimensions;
}

Vec<float> Image::getPhysicalDims()
{
	return physicalDims;
}
void Image::setPhysicalDim(Vec<float> pDims)
{
	this->physicalDims = pDims;
}