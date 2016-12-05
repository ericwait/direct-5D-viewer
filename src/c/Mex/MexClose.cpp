#include "MexCommand.h"
#include "Global/Globals.h"
#include "Messages/Threads.h"

void MexClose::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.writeMessage("close", NULL);
	cleanUp();
	while(!gMsgQueueToMex.doneLoading() && !gMsgQueueToMex.isempty())
	{
		;
	}
	gMsgQueueToMex.clear();
	gMsgQueueToDirectX.clear();
}

std::string MexClose::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexClose::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
}

void MexClose::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("Will close the DirectX viewer.");
}