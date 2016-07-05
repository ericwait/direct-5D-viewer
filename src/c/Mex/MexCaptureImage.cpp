#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexCaptureImage::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	std::string fileNameOut;
	HRESULT hr;

	if (nrhs > 2)
	{
		char dirBuff[512];
		char filePreBuff[256];

		mxGetString(prhs[0], dirBuff, 512);
		mxGetString(prhs[1], dirBuff, 256);

		hr = gRenderer->captureWindow(dirBuff, filePreBuff, fileNameOut);
	}
	else
	{
		hr = gRenderer->captureWindow(&fileNameOut);
	}

	if (FAILED(hr)) mexErrMsgTxt("Unable to capture the screen!");

	plhs[0] = mxCreateString(fileNameOut.c_str());
}

std::string MexCaptureImage::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nlhs != 0)
		return "There must be one output argument to hold the file path/name that was captured!";

	return "";
}

void MexCaptureImage::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	//inArgs.push_back("arrowFaces");
	//inArgs.push_back("arrowVerts");
}

void MexCaptureImage::help(std::vector<std::string>& helpLines) const
{
//	helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");
//
//	helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
//	helpLines.push_back("\tArrowVerts -- This is a list of vertices that make up the polygon for each axis.");
}