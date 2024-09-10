#define MW_NEEDS_VERSION_H  // Define this macro before including mex.h

#include "MexCommand.h"
#include "Globals.h"
#include "Threads.h"
#include "LoadMessages.h"

// Now MEXFUNCTION_LINKAGE will include __declspec(dllexport) or equivalent
MEXFUNCTION_LINKAGE void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    MexCommand::run(nlhs, plhs, nrhs, prhs);
    return;
}

extern "C" MEXFUNCTION_LINKAGE void exitFunc()
{
    gMsgQueueToDirectX.pushMessage(new MessageClose(), true);
    cleanUp();
    gMsgQueueToMex.clear();
    gMsgQueueToDirectX.clear();
}