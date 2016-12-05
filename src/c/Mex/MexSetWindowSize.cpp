#include "MexCommand.h"
#include "MexFunctions.h"
#include "Global/Globals.h"

void MexSetWindowSize::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double* dims = new double[2];
	dims[0] = mxGetScalar(prhs[0]);
	dims[1] = mxGetScalar(prhs[1]);

	gMsgQueueToDirectX.writeMessage("windowSize", (void*)dims);
}

std::string MexSetWindowSize::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nrhs!=2)
		return "Not the right arguments for play!";

	return "";
}

void MexSetWindowSize::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("width");
	inArgs.push_back("height");
}

void MexSetWindowSize::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This set the viewable space to the number of pixels specified.");

	helpLines.push_back("\tWidth - This is how many pixels the viewer should be in width.");
	helpLines.push_back("\tHeight - This is how many pixels the viewer should be in width.");
}