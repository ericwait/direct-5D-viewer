#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexViewTexture::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	char buff[96];
	mxGetString(prhs[0], buff, 96);

	GraphicObjectTypes* textureType = new GraphicObjectTypes;// GraphicObjectTypes::OriginalVolume;

	if (_strcmpi("original", buff) == 0)
		*textureType = GraphicObjectTypes::OriginalVolume;
	else if (_strcmpi("processed", buff) == 0)
		*textureType = GraphicObjectTypes::ProcessedVolume;

	dataQueue->writeMessage("viewTexture", (void*)textureType);
}

std::string MexViewTexture::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for viewTexture!";

	return "";
}

void MexViewTexture::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	//inArgs.push_back("arrowFaces");
}

void MexViewTexture::help(std::vector<std::string>& helpLines) const
{
	//helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	//helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
}