#include "MexCommand.h"
#include "MexFunctions.h"
#include "Global/Globals.h"

void MexToggleWireframe::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double onD = mxGetScalar(prhs[0]);
	double* onD2 = new double;
	*onD2 = onD;
	gMsgQueueToDirectX.writeMessage("ToggleWireframe", (void*)onD2);
}

std::string MexToggleWireframe::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for ToggleWireframe!";

	return "";
}

void MexToggleWireframe::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("wireFrameOn");
}

void MexToggleWireframe::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will toggle wireframe on or off for the polygons.");

	helpLines.push_back("\tWireFrameOn - This is a double that is either a zero or a one, where a zero means filled polygons and a one means wire framed polygons.");
}