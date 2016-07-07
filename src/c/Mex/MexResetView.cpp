#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexResetView::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.writeMessage("resetView", NULL);
}

std::string MexResetView::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexResetView::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
}

void MexResetView::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will reset the camera to its default position and view.");
}