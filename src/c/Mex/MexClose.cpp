#include "MexCommand.h"
#include "Globals.h"
#include "Threads.h"

#include "LoadMessages.h"

void MexClose::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.pushMessage(new MessageClose(), true);

	cleanUp();

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