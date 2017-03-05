#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/AnimMessages.h"

void MexSetCaptureSize::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	int width = (int)mxGetScalar(prhs[0]);
	int height = (int)mxGetScalar(prhs[1]);

	gMsgQueueToDirectX.pushMessage(new MessageSetCaptureSize(width, height));
}

std::string MexSetCaptureSize::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if ( nrhs!=2 )
		return "Incorrect number of arguments!";

	if ( !mxIsScalar(prhs[0]) )
		return "Width must be a single scalar number!";

	if ( !mxIsScalar(prhs[1]) )
		return "Height must be a single scalar number!";

	return "";
}

void MexSetCaptureSize::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("width");
	inArgs.push_back("height");
}

void MexSetCaptureSize::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This sets the pixel dimensions of capture images.");

	helpLines.push_back("\tWidth - This is how many pixels captured images should be in width.");
	helpLines.push_back("\tHeight - This is how many pixels captured images should be in width.");
}