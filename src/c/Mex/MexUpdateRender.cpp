#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/RenderMessages.h"

void MexUpdateRender::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(gMsgQueueToMex.hasError())
        return;

    gMsgQueueToDirectX.pushMessage(new MessageUpdateRender());
}

std::string MexUpdateRender::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    return "";
}

void MexUpdateRender::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{}

void MexUpdateRender::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("\tForces the display to update.");
}
