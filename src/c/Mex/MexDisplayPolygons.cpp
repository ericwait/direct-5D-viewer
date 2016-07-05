#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexDisplayPolygons::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double* hullList = (double*)mxGetData(prhs[0]);
	size_t numPolygons = mxGetNumberOfElements(prhs[0]);

	std::set<int>* polygonset = new std::set<int>;
	for (size_t i = 0; i < numPolygons; ++i)
		polygonset->insert((int)(hullList[i]));

	gDataQueue->writeMessage("displayPolygons", (void*)polygonset);
}

std::string MexDisplayPolygons::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 2)
		return "Not the right arguments for displayPolygons!";

	return "";
}

void MexDisplayPolygons::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	//inArgs.push_back("arrowFaces");
}

void MexDisplayPolygons::help(std::vector<std::string>& helpLines) const
{
	/*helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");*/
}