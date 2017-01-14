#include "MexCommand.h"
#include "Global/Globals.h"

void MexZoomDecrement::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    double speedFactorM = mxGetScalar(prhs[0]);
    double* speed = new double;
    *speed = speedFactorM;

    gMsgQueueToDirectX.writeMessage("zoomDecrement", speed);
}

std::string MexZoomDecrement::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(nrhs!=1)
        return "Not the right arguments for ZoomDecrement!";

    return "";
}

void MexZoomDecrement::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
    inArgs.push_back("speedFactor");
}

void MexZoomDecrement::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will zoom the camera out.");

    helpLines.push_back("\tSpeedFactor -- this is a multiplier on how fast or how big the steps are for each zoom.");
}
