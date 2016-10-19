#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexMoveUp::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    gMsgQueueToDirectX.writeMessage("moveUp", NULL);
}

std::string MexMoveUp::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    return "";
}

void MexMoveUp::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{}

void MexMoveUp::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will move the camera up by one step.");
}