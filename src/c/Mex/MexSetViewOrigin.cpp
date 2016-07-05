#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexSetViewOrigin::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double* origin = (double*)mxGetData(prhs[0]);
	size_t numDims = mxGetNumberOfElements(prhs[10]);

	if (numDims != 3) mexErrMsgTxt("There needs to be three doubles for the view origin!");

	double* originMsg = new double[3];
	originMsg[0] = origin[0];
	originMsg[1] = origin[1];
	originMsg[2] = origin[2];

	gDataQueue->writeMessage("setViewOrigin", (void*)originMsg);
}

std::string MexSetViewOrigin::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 2)
		return "Not the right arguments for setViewOrigin!";

	return "";
}

void MexSetViewOrigin::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	//inArgs.push_back("arrowFaces");
}

void MexSetViewOrigin::help(std::vector<std::string>& helpLines) const
{
	//helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	//helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
}