#define DLL_EXPORT_SYM __declspec(dllexport)
#include "MexCommand.h"
#include "..\Global\Globals.h"
#include "..\Messages\Threads.h"

MEXFUNCTION_LINKAGE void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	MexCommand::run(nlhs, plhs, nrhs, prhs);

	return;
}

extern "C" void exitFunc()
{
	gMsgQueueToDirectX.writeMessage("close", NULL);
	cleanUp();
	while(!gMsgQueueToMex.doneLoading())
	{
		;
	}
	gMsgQueueToMex.clear();
	gMsgQueueToDirectX.clear();
}