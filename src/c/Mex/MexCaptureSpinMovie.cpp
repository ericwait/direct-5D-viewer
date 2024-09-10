#include "MexCommand.h"
#include "Globals.h"

#include "AnimMessages.h"

void MexCaptureSpinMovie::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.pushMessage(new MessageCaptureSpinMovie());
}

std::string MexCaptureSpinMovie::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexCaptureSpinMovie::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
}

void MexCaptureSpinMovie::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will capture 720 images with the current view spinning 0.5 degrees per frame");
}