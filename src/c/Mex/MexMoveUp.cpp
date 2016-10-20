#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexMoveUp::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    double speedFactorM = mxGetScalar(prhs[0]);
    double* speed = new double;
    *speed = speedFactorM;

    gMsgQueueToDirectX.writeMessage("moveUp", NULL);
}

std::string MexMoveUp::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(nrhs!=1)
        return "Not the right arguments for MoveUp!";

    return "";
}

void MexMoveUp::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
    inArgs.push_back("speedFactor");
}

void MexMoveUp::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will move the camera up by one step.");

    helpLines.push_back("\tSpeedFactor -- this is a multiplier on how fast or how big the steps are for each move.");
}