#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexResetView::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	std::string s = "resetView";
	gDataQueue->writeMessage(s, NULL);
}

std::string MexResetView::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexResetView::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	//inArgs.push_back("arrowFaces");
}

void MexResetView::help(std::vector<std::string>& helpLines) const
{
	//helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	//helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
}