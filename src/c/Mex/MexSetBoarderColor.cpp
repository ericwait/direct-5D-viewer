#include "MexCommand.h"
#include "Globals.h"
#include "ViewMessages.h"

void MexSetBorderColor::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    double* bc = (double*)mxGetData(prhs[0]);
    Vec<float> background(bc);

    gMsgQueueToDirectX.pushMessage(new MessageSetBorderColor(background));
}

std::string MexSetBorderColor::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(nrhs!=1)
        return "There must be one input consisting of three doubles between [0,1]!";

    return "";
}

void MexSetBorderColor::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
    inArgs.push_back("color");
}

void MexSetBorderColor::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("This will set the color of the border box.");

    helpLines.push_back("\tColor - This should be three doubles between [0,1] that represent (r,g,b) of the desired background color.");
}