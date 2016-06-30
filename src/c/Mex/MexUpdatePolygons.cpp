#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexUpdatePolygons::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	const mxArray* polygons = prhs[0];
	if (polygons == NULL) mexErrMsgTxt("No polygons passed as the second argument!\n");

	HRESULT hr = updatePolygons(polygons);
	if (FAILED(hr))
		mexErrMsgTxt("Could not update polygons!");
}

std::string MexUpdatePolygons::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for loadPolygons!";

	return "";
}

void MexUpdatePolygons::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	/*inArgs.push_back("arrowFaces");*/
}

void MexUpdatePolygons::help(std::vector<std::string>& helpLines) const
{
	//helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	//helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
}