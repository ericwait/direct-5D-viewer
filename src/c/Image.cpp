#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"
#include "Image.h"

Image::Image(int numChannels, int numFrames, Vec<size_t> dimensions){
	this->dimensions = dimensions;
	this->numChannels = numChannels;
	this->numFrames = numFrames;

	size_t pixelsCount = dimensions.product();
	pixels = (unsigned char*)malloc(pixelsCount);
}

Image::~Image(){
	free(pixels);
	free(physicalDim);
}