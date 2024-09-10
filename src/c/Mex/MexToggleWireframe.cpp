#include "MexCommand.h"
#include "Globals.h"

#include "ViewMessages.h"

void MexToggleWireframe::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	bool wireframe = (mxGetScalar(prhs[0]) != 0.0);
	gMsgQueueToDirectX.pushMessage(new MessageSetPolyWireframe(wireframe));
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