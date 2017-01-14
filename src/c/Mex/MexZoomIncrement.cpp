#include "MexCommand.h"
#include "Global/Globals.h"

void MexZoomIncrement::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    double speedFactorM = mxGetScalar(prhs[0]);
    double* speed = new double;
    *speed = speedFactorM;

    gMsgQueueToDirectX.writeMessage("zoomIncrement", speed);
}

std::string MexZoomIncrement::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(nrhs!=1)
        return "Not the right arguments for ZoomIncrement!";

    return "";
}

void MexZoomIncrement::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
    inArgs.push_back("speedFactor");
}

void MexZoomIncrement::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will zoom the camera in.");

    helpLines.push_back("\tSpeedFactor -- this is a multiplier on how fast or how big the steps are for each zoom.");
}
