#include "MexCommand.h"
#include "Globals.h"

#include "ViewMessages.h"

void MexShowFrameNumber::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	bool frameOn = (mxGetScalar(prhs[0]) != 0.0);
	gMsgQueueToDirectX.pushMessage(new MessageShowFrame(frameOn));
}

std::string MexShowFrameNumber::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nrhs!=1)
		return "Not the right arguments for showFrameNumber!";

	return "";
}

void MexShowFrameNumber::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("on");
}

void MexShowFrameNumber::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will toggle the FrameNumber on and off.");

	helpLines.push_back("\tOn - If this is set to one, then the FrameNumber will be on. If this is 0, the FrameNumber will be off.");
}