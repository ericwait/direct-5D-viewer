#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexRotate::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double onD = mxGetScalar(prhs[0]);
	double* onD2 = new double;
	*onD2 = onD;
	gMsgQueueToDirectX.writeMessage("rotate", (void*)onD2);
}

std::string MexRotate::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for rotate!";

	return "";
}

void MexRotate::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("on");
}

void MexRotate::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will start the volume rotating about the view aligned y axis.");

	helpLines.push_back("\tOn - If this is set to one, then the volume will rotate. If this is 0, the volume will not rotate.");
}