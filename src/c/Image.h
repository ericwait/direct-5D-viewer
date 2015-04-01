#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"

class Image
{
public:
	unsigned char* pixels;
	Vec<size_t> dimensions;
	int numChannels;
	int numFrames;
	double* physicalDim;

	Image(int numChannels, int numFrames, Vec<size_t> dimensions);
	~Image();
};