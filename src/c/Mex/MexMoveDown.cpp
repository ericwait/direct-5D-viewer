#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexMoveDown::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    gMsgQueueToDirectX.writeMessage("moveDown", NULL);
}

std::string MexMoveDown::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    return "";
}

void MexMoveDown::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{}

void MexMoveDown::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will move the camera down by one step.");
}