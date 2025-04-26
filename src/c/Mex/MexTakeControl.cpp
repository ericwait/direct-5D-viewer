#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/AnimMessages.h"

void MexTakeControl::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.pushMessage(new MessageTakeControl(),true);
}

std::string MexTakeControl::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexTakeControl::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
}

void MexTakeControl::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will stop the renderer until the 'ReleaseControl' command is called.");
}