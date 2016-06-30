#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexSetCapturePath::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	char filePath[512];
	char fileName[255];
	mxGetString(prhs[0], filePath, 512);
	mxGetString(prhs[1], fileName, 255);

	gRenderer->setCaptureFilePath(filePath);
	gRenderer->setCaptureFileName(fileName);
}

std::string MexSetCapturePath::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 2)
		return "Not the right arguments for setCapturePath!";

	return "";
}

void MexSetCapturePath::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	//inArgs.push_back("arrowFaces");
	//inArgs.push_back("arrowVerts");
}

void MexSetCapturePath::help(std::vector<std::string>& helpLines) const
{
	//helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	//helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
	//helpLines.push_back("\tArrowVerts -- This is a list of vertices that make up the polygon for each axis.");
}