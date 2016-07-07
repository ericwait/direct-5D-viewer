#include "MexCommand.h"
#include "..\Global\Globals.h"
#include "..\Messages\Threads.h"

void MexClose::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
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

std::string MexClose::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexClose::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
}

void MexClose::help(std::vector<std::string>& helpLines) const
{
	//helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");
}