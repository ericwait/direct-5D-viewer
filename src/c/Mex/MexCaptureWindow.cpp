#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexCaptureWindow::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.writeMessage("CaptureWindow", NULL);
}

std::string MexCaptureWindow::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexCaptureWindow::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
}

void MexCaptureWindow::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will capture the current view to either the current directory or the directory specified in 'SetCapturePath'.");
}
