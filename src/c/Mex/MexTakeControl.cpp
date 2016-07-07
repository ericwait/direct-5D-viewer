#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexTakeControl::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.writeMessage("takeControl", NULL);
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