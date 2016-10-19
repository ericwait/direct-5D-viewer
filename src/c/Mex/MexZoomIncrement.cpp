#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexZoomIncrement::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    gMsgQueueToDirectX.writeMessage("zoomIncrement", NULL);
}

std::string MexZoomIncrement::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    return "";
}

void MexZoomIncrement::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{}

void MexZoomIncrement::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will zoom the camera in by one step.");
}