#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexMoveRight::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    gMsgQueueToDirectX.writeMessage("moveRight", NULL);
}

std::string MexMoveRight::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    return "";
}

void MexMoveRight::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{}

void MexMoveRight::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will move the camera right by one step.");
}