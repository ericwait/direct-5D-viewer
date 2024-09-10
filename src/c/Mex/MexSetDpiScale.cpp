#include "MexCommand.h"
#include "Globals.h"

#include "ViewMessages.h"

void MexSetDpiScale::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	int dpiScale = (int)mxGetScalar(prhs[0]);
	gMsgQueueToDirectX.pushMessage(new MessageSetDpiScale(dpiScale));
}

std::string MexSetDpiScale::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Invalid number of arguments";

	return "";
}

void MexSetDpiScale::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("scalePct");
}

void MexSetDpiScale::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This sets the amount to enlarge each window pixel (larger scale percentages improve render speed but can blur the rendering).");

	helpLines.push_back("\tScalePct - The percentage to 'enlarge' each pixel in the window (default 150%).");
}
