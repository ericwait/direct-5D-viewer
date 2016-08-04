#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexShowFrameNumber::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    double onD = mxGetScalar(prhs[0]);
    double* onD2 = new double;
    *onD2 = onD;
    gMsgQueueToDirectX.writeMessage("showFrameNumber", (void*)onD2);
}

std::string MexShowFrameNumber::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(nrhs!=1)
        return "Not the right arguments for showFrameNumber!";

    return "";
}

void MexShowFrameNumber::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
    inArgs.push_back("on");
}

void MexShowFrameNumber::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will toggle the FrameNumber on and off.");

    helpLines.push_back("\tOn - If this is set to one, then the FrameNumber will be on. If this is 0, the FrameNumber will be off.");
}