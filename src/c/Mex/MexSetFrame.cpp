#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexSetFrame::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	int curFrame = (int)mxGetScalar(prhs[0]);
	int* onD2 = new int;
	*onD2 = curFrame - 1;
	dataQueue->writeMessage("setFrame", (void*)onD2);
}

std::string MexSetFrame::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for setFrame!";

	return "";
}

void MexSetFrame::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	/*inArgs.push_back("arrowFaces");*/
}

void MexSetFrame::help(std::vector<std::string>& helpLines) const
{
	//helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	//helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
}