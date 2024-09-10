#include "MexCommand.h"
#include "Globals.h"

#include "ViewMessages.h"

void MexResetView::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.pushMessage(new MessageResetView());
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