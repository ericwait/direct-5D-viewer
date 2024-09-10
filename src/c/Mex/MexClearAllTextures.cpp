#include "MexCommand.h"
#include "Globals.h"

#include "LoadMessages.h"
#include "RenderMessages.h"

void MexClearAllTextures::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    if(gMsgQueueToMex.hasError())
        return;

    GraphicObjectTypes texType = GraphicObjectTypes::OriginalVolume;
    if(nrhs>0)
    {
        char buff[96];
        mxGetString(prhs[0], buff, 96);

        if(_strcmpi("original", buff)==0)
            texType = GraphicObjectTypes::OriginalVolume;
        else if(_strcmpi("processed", buff)==0)
            texType = GraphicObjectTypes::ProcessedVolume;
    }

    gMsgQueueToDirectX.pushMessage(new MessageClearAllTexture(texType));
}

std::string MexClearAllTextures::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
    return "";
}

void MexClearAllTextures::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
    inArgs.push_back("BufferType");
}

void MexClearAllTextures::help(std::vector<std::string>& helpLines) const
{
    helpLines.push_back("\tBufferType -- this can either be 'original' or 'processed' and corresponds to the first and second texture buffer available to clear from.");
}
