#include "Global/Globals.h"
#include "mex.h"
#include "D3d/MessageProcessor.h"
#include "Mex/MexFunctions.h"
#include "Image.h"

Image::Image(int numChannels,int numFrames,Vec<size_t> dimensions)
{
	setDefauts();

	this->dimensions = dimensions;
	this->numChannels = numChannels;
	this->numFrames = numFrames;
}

void Image::setDefauts()
{
	textureType = GraphicObjectTypes::OriginalVolume;
	dimensions = Vec<size_t>(0,0,0);
	numChannels = 0;
	numFrames = 0;
	physicalDims = Vec<float>(0.0f,0.0f,0.0f);
	pixels = NULL;
}

Image::~Image()
{
	setDefauts();
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
void Image::setPixels(unsigned char* pixelsIn)
{
	this->pixels = pixelsIn;
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