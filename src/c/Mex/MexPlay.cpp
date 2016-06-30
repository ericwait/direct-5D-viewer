#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexPlay::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double onD = mxGetScalar(prhs[0]);
	double* onD2 = new double;
	*onD2 = onD;
	dataQueue->writeMessage("play", (void*)onD2);
}

std::string MexPlay::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for play!";

	return "";
}

void MexPlay::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	//inArgs.push_back("arrowFaces");
}

void MexPlay::help(std::vector<std::string>& helpLines) const
{
	/*helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");*/
}