#include "MexCommand.h"
#include "Global/Globals.h"

void MexShowScaleBar::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    double onD = mxGetScalar(prhs[0]);
    double* onD2 = new double;
    *onD2 = onD;
    gMsgQueueToDirectX.writeMessage("showScaleBar", (void*)onD2);
}

std::string MexShowScaleBar::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(nrhs!=1)
        return "Not the right arguments for showScaleBar!";

    return "";
}

void MexShowScaleBar::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
    inArgs.push_back("on");
}

void MexShowScaleBar::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will toggle the ScaleBar on and off.");

    helpLines.push_back("\tOn - If this is set to one, then the ScaleBar will be on. If this is 0, the ScaleBar will be off.");
}