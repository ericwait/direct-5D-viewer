#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexMoveDown::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    double speedFactorM = mxGetScalar(prhs[0]);
    double* speed = new double;
    *speed = speedFactorM;

    gMsgQueueToDirectX.writeMessage("moveDown", NULL);
}

std::string MexMoveDown::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(nrhs!=1)
        return "Not the right arguments for MoveDown!";

    return "";
}

void MexMoveDown::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
    inArgs.push_back("speedFactor");
}

void MexMoveDown::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will move the camera down by one step.");

    helpLines.push_back("\tSpeedFactor -- this is a multiplier on how fast or how big the steps are for each move.");
}