#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/LoadMessages.h"
#include "Messages/RenderMessages.h"

void MexClearTextureFrame::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(gMsgQueueToMex.hasError())
        return;

    int frame = (int)mxGetScalar(prhs[0]);

    GraphicObjectTypes texType = GraphicObjectTypes::OriginalVolume;
    if(nrhs>1)
    {
        char buff[96];
        mxGetString(prhs[1], buff, 96);

        if(_strcmpi("original", buff)==0)
            texType = GraphicObjectTypes::OriginalVolume;
        else if(_strcmpi("processed", buff)==0)
            texType = GraphicObjectTypes::ProcessedVolume;
    }

    gMsgQueueToDirectX.pushMessage(new MessageClearTextureFrame(texType,frame));
}

std::string MexClearTextureFrame::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(nrhs<1)
        return "Frame must be specified!.";

    return "";
}

void MexClearTextureFrame::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
    inArgs.push_back("Frame");
    inArgs.push_back("BufferType");
}

void MexClearTextureFrame::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("\tFrame -- The frame in the sequence at which to clear the texture data.");
    helpLines.push_back("\tBufferType -- this can either be 'original' or 'processed' and corresponds to the first and second texture buffer available to clear from.");
}
