#include "MexCommand.h"
#include "MexFunctions.h"
#include "Global/Globals.h"

void MexShowTexture::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	char buff[96];
	mxGetString(prhs[0], buff, 96);

	GraphicObjectTypes* textureType = new GraphicObjectTypes;// GraphicObjectTypes::OriginalVolume;

	if (_strcmpi("original", buff) == 0)
		*textureType = GraphicObjectTypes::OriginalVolume;
	else if (_strcmpi("processed", buff) == 0)
		*textureType = GraphicObjectTypes::ProcessedVolume;

	gMsgQueueToDirectX.writeMessage("ViewTexture", (void*)textureType);
}

std::string MexShowTexture::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for ShowTexture!";

	return "";
}

void MexShowTexture::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("bufferType");
}

void MexShowTexture::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This selects which of the two buffers to display.");

	helpLines.push_back("\tBufferType - there are two buffers available to be displayed, 'original' and 'processed'.");
	helpLines.push_back("\t\tOriginal - corresponds to the first buffer and can be displayed if an image has been loaded.");
	helpLines.push_back("\t\tProcessed - corresponds to the second buffer and can be displayed if an image has been loaded.");
}