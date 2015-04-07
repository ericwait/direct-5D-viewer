#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"

class Image
{
public:
	Image(int numChannels, int numFrames, Vec<size_t> dimensions, char buff[96]);
	~Image();

	unsigned char* getPixels();
	void setPixels(unsigned char* pixels);

	int getNumChannels();
	void setNumChannels(int numChannels);

	int getNumFrames();
	void setNumFrames(int numFrames);

	Vec<size_t> getDimensions();
	void setDimensions(Vec<size_t> dimensions);

	double* getPhysicalDim();
	void setPhysicalDim();

	char* getBuff();
	void setBuff(char buff[96]);

	void setNumArgs(int numArgs);
	int getNumArgs();

private:
	int numArgs;
	unsigned char* pixels;
	Vec<size_t> dimensions;
	void Dimensions(Vec<size_t> val) { dimensions = val; }
	int numChannels;
	int numFrames;
	double* physicalDim;
	char* buff;
};