#include "MexCommand.h"
#include "Globals.h"

#include "AnimMessages.h"

void MexReleaseControl::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.pushMessage(new MessageReleaseControl());
}

std::string MexReleaseControl::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexReleaseControl::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
}

void MexReleaseControl::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will allow the renderer to start up again.");
}