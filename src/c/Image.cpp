#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"
#include "Image.h"

Image::Image(int numChannels, int numFrames, Vec<size_t> dimensions, char buff[96]){
	this->dimensions = dimensions;
	this->numChannels = numChannels;
	this->numFrames = numFrames;
	this->buff = buff;

	size_t pixelsCount = dimensions.product() * numFrames * numChannels;
	pixels = (unsigned char*)malloc(pixelsCount);
}

Image::~Image(){
	free(pixels);
}

char* Image::getBuff(){
	return buff;
}
void Image::setBuff(char buff[96]){
	this->buff = buff;
}

int Image::getNumArgs(){
	return numArgs;
}
void Image::setNumArgs(int numArgs){
	this->numArgs = numArgs;
}

unsigned char* Image::getPixels(){
	return pixels;
}
void Image::setPixels(unsigned char* pixels){
	memcpy(this->pixels,pixels, dimensions.product()* numChannels * numFrames * sizeof(unsigned char));
}

int Image::getNumChannels(){
	return numChannels;
}
void Image::setNumChannels(int numChannels){
	this->numChannels = numChannels;
}

int Image::getNumFrames(){
	return numFrames;
}
void Image::setNumFrames(int numFrames){
	this->numFrames = numFrames;
}

Vec<size_t> Image::getDimensions(){
	return dimensions;
}
void Image::setDimensions(Vec<size_t> dimensions){
	this->dimensions = dimensions;
}

Vec<float> Image::getPhysicalDims(){
	return physicalDims;
}
void Image::setPhysicalDim(Vec<float> pDims)
{
	this->physicalDims = pDims;
}