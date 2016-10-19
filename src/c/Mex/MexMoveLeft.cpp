#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexMoveLeft::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    gMsgQueueToDirectX.writeMessage("moveLeft", NULL);
}

std::string MexMoveLeft::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    return "";
}

void MexMoveLeft::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{}

void MexMoveLeft::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will move the camera left by one step.");
}