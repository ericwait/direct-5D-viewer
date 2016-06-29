#define DLL_EXPORT_SYM __declspec(dllexport)
#include "MexCommand.h"

MEXFUNCTION_LINKAGE void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	MexCommand::run(nlhs, plhs, nrhs, prhs);

	return;
}
