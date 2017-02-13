#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/LoadMessages.h"

void MexLoadTexture::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	// check the message queue for an error message before continuing

	if(gMsgQueueToMex.hasError())
		return;

	const mwSize* DIMS = mxGetDimensions(prhs[0]);

	// Dimensions 1-5
	Vec<size_t> dims = Vec<size_t>(DIMS[0], DIMS[1], DIMS[2]);
	int numChannels = 1;
	int numFrames = 1;
	
	size_t numDims = mxGetNumberOfDimensions(prhs[0]);
	if (numDims>3)
		numChannels = int(DIMS[3]);

	if (numDims > 4)
		numFrames = int(DIMS[4]);

	// ptr to image data
	unsigned char* image = (unsigned char*)mxGetData(prhs[0]);

	GraphicObjectTypes texType = GraphicObjectTypes::OriginalVolume;
	if (nrhs > 1)
	{
		char buff[96];
		mxGetString(prhs[1], buff, 96);

		if (_strcmpi("original", buff) == 0)
			texType = GraphicObjectTypes::OriginalVolume;
		else if (_strcmpi("processed", buff) == 0)
			texType = GraphicObjectTypes::ProcessedVolume;
	}

	gMsgQueueToDirectX.pushMessage(new MessageLoadTexture(texType, image), true);
}

std::string MexLoadTexture::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (!mxIsClass(prhs[0], "uint8"))
		return "Image must be of uint8 class!";

	size_t numDims = mxGetNumberOfDimensions(prhs[0]);
	if (numDims<3)
		return "Image must have at least three dimensions!";

	return "";
}

void MexLoadTexture::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("Image");
	inArgs.push_back("BufferType");
}

void MexLoadTexture::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("this will load the image into texture buffers for display in the D3d viewer.");

	helpLines.push_back("\tImage -- This should be an matrix up to five dimensions in the order (y,x,z,channel,time).");
	helpLines.push_back("\tBufferType -- this can either be 'original' or 'processed' and corresponds to the first and second texture buffer available to load images into.");
}