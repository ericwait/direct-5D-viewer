#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/ViewMessages.h"

void MexSetFrontClip::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double clipDistance = mxGetScalar(prhs[0]);
	gMsgQueueToDirectX.pushMessage(new MessageSetFrontClipPlane((float)clipDistance));
	gMsgQueueToDirectX.pushMessage(new MessageUpdateRender());
}

std::string MexSetFrontClip::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if ( nrhs != 1 )
		return "Must specify the front clipping distance.";

	if ( !mxIsScalar(prhs[0]) )
		return "Clipping distance must be a scalar value.";

	return "";
}

void MexSetFrontClip::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("FrontClipDistance");
}

void MexSetFrontClip::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will set the front clipping distance.");

	helpLines.push_back("\tFrontClipDistance - The distance in front of the camera to clip.");
}