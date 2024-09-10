#include "MexCommand.h"
#include "Globals.h"

#include "ViewMessages.h"

void MexSetViewOrigin::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	Vec<double> origin((double*)mxGetData(prhs[0]));
	gMsgQueueToDirectX.pushMessage(new MessageSetViewOrigin(origin));
}

std::string MexSetViewOrigin::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for setViewOrigin!";

	double* origin = (double*)mxGetData(prhs[0]);
	size_t numDims = mxGetNumberOfElements(prhs[0]);

	if(numDims!=3) 
		return "There needs to be three doubles for the view origin!";

	return "";
}

void MexSetViewOrigin::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("viewOrigin");
}

void MexSetViewOrigin::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will set the view origin (where the camera looks).");

	helpLines.push_back("\tViewOrigin - this is three values that correspond to the point that you would like the camera to point at.");
}