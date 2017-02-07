#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/ViewMessages.h"

void MexShowTexture::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	char buff[96];
	mxGetString(prhs[0], buff, 96);

	GraphicObjectTypes showType = GraphicObjectTypes::OriginalVolume;

	if ( _strcmpi("original", buff) == 0 )
		showType = GraphicObjectTypes::OriginalVolume;
	else if ( _strcmpi("processed", buff) == 0 )
		showType = GraphicObjectTypes::ProcessedVolume;

	for ( int i=GraphicObjectTypes::OriginalVolume; i < GraphicObjectTypes::VTend; ++i )
		gMsgQueueToDirectX.pushMessage(new MessageShowObjectType((GraphicObjectTypes)i, (showType == i)));

	gMsgQueueToDirectX.pushMessage(new MessageUpdateRender());
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