#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/LoadMessages.h"

void MexLoadTextureFrame::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	// check the message queue for an error message before continuing

	if ( gMsgQueueToMex.hasError() )
		return;

	const mwSize* DIMS = mxGetDimensions(prhs[0]);

	// Dimensions 1-5
	Vec<size_t> dims = Vec<size_t>(DIMS[0], DIMS[1], DIMS[2]);
	int numChannels = 1;
	int numFrames = 1;

	size_t numDims = mxGetNumberOfDimensions(prhs[0]);
	if ( numDims>3 )
		numChannels = int(DIMS[3]);

	int frame = (int)mxGetScalar(prhs[1]);

	// ptr to image data
	unsigned char* image = (unsigned char*)mxGetData(prhs[0]);

	GraphicObjectTypes texType = GraphicObjectTypes::OriginalVolume;
	if ( nrhs > 2 )
	{
		char buff[96];
		mxGetString(prhs[2], buff, 96);

		if ( _strcmpi("original", buff) == 0 )
			texType = GraphicObjectTypes::OriginalVolume;
		else if ( _strcmpi("processed", buff) == 0 )
			texType = GraphicObjectTypes::ProcessedVolume;
	}

	gMsgQueueToDirectX.pushMessage(new MessageLoadTextureFrame(texType, MAT_TO_C(frame), image), true);
}

std::string MexLoadTextureFrame::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if ( !mxIsClass(prhs[0], "uint8") )
		return "Image must be of uint8 class!";

	size_t numDims = mxGetNumberOfDimensions(prhs[0]);
	if ( numDims<3 )
		return "Image must have at least three dimensions!";

	if ( numDims > 4 )
		return "Image data cannot have more than 4 dimensions (x,y,z,chan).";

	if ( nrhs < 2 )
		return "Frame must be specified!.";

	if ( mxGetNumberOfElements(prhs[1]) > 1 )
		return "Only a single frame of texture data can be loaded!";

	return "";
}

void MexLoadTextureFrame::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("Image");
	inArgs.push_back("Frame");
	inArgs.push_back("BufferType");
}

void MexLoadTextureFrame::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("this will load the image into texture buffers for display in the D3d viewer.");

	helpLines.push_back("\tImage -- This should be an matrix up to 4 dimensions in the order (y,x,z,channel).");
	helpLines.push_back("\tFrame -- The frame in the sequence at which to load this texture data.");
	helpLines.push_back("\tBufferType -- this can either be 'original' or 'processed' and corresponds to the first and second texture buffer available to load images into.");
}