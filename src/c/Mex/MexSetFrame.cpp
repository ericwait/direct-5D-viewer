#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexSetFrame::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	int curFrame = (int)mxGetScalar(prhs[0]);
	int* onD2 = new int;
	*onD2 = curFrame - 1;
	gMsgQueueToDirectX.writeMessage("setFrame", (void*)onD2);
}

std::string MexSetFrame::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for setFrame!";

	return "";
}

void MexSetFrame::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("frame");
}

void MexSetFrame::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will set the currently viewed frame to the one specified.");

	helpLines.push_back("\tFrame - This is the frame that should be displayed. The first frame starts at 1.");
}