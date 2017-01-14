#include "MexGlobals.h"
#include "MexCommand.h"
#include "Widget.h"
#include "Global/Globals.h"
#include "Messages/Threads.h"

void MexInit::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	char buff[512];
	mxGetString(prhs[6], buff, 512);
	std::string rootDir = buff;
	startThread(rootDir);

	mexAtExit(exitFunc);

	loadWidget(prhs);
	gRendererOn = true;
}

std::string MexInit::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs < 7)
		return "Incorrect number of inputs!";

	return "";
}

void MexInit::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("arrowFaces");
	inArgs.push_back("arrowVerts");
	inArgs.push_back("arrowNorms");
	inArgs.push_back("sphereFaces");
	inArgs.push_back("sphereVerts");
	inArgs.push_back("shereNorms");
	inArgs.push_back("pathStr");
}

void MexInit::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
	helpLines.push_back("\tArrowVerts -- This is a list of vertices that make up the polygon for each axis.");
	helpLines.push_back("\tArrowNorms -- This is the normal directions that correspond to the faces for each axis.");
	helpLines.push_back("\tSphereFaces -- This is an ordered list of vertices that make up the facets (triangles) for an object that represents the axis origin.");
	helpLines.push_back("\tSphereVerts -- This is a list of vertices that make up the polygon for an object that represents the axis origin.");
	helpLines.push_back("\tShereNorms -- This is the normal directions that correspond to the faces for an object that represents the axis origin.");
	helpLines.push_back("\tPathStr -- This is the path where the render will find the shader files and is typically in the same directory as the mex dll");
}