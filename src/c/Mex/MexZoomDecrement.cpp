#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexZoomDecrement::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    gMsgQueueToDirectX.writeMessage("zoomDecrement", NULL);
}

std::string MexZoomDecrement::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    return "";
}

void MexZoomDecrement::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{}

void MexZoomDecrement::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will zoom the camera out by one step.");
}