#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexTransferUpdate::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	char buff[96];
	if (nrhs > 2)
	{
		mxGetString(prhs[1], buff, 96);
	}

	size_t numElem = mxGetNumberOfElements(prhs[0]);

	for (int chan = 0; chan < numElem; ++chan)
	{
		// TODO Pull these out.. eventually 
		Vec<float> transferFunction(0.0f, 0.0f, 0.0f);
		Vec<float> ranges;
		Vec<float> color;
		float alphaMod;

		mxArray* mxColorPt = mxGetField(prhs[1], chan, "color");
		double* mxColor = (double*)mxGetData(mxColorPt);
		color = Vec<float>((float)(mxColor[0]), (float)(mxColor[1]), (float)(mxColor[2]));

		mxArray* mxAPt = mxGetField(prhs[0], chan, "a");
		mxArray* mxBPt = mxGetField(prhs[0], chan, "b");
		mxArray* mxCPt = mxGetField(prhs[0], chan, "c");
		double a = mxGetScalar(mxAPt);
		double b = mxGetScalar(mxBPt);
		double c = mxGetScalar(mxCPt);
		transferFunction = Vec<float>((float)a, (float)b, (float)c);

		mxArray* mxMin = mxGetField(prhs[0], chan, "minVal");
		mxArray* mxMax = mxGetField(prhs[0], chan, "maxVal");
		ranges = Vec<float>((float)mxGetScalar(mxMin), (float)mxGetScalar(mxMax), 1.0f);

		mxArray* mxAlphaPt = mxGetField(prhs[0], chan, "alphaMod");
		mxArray* mxOnPt = mxGetField(prhs[0], chan, "visible");
		if (mxGetScalar(mxOnPt) != 0)
			alphaMod = (float)mxGetScalar(mxAlphaPt);
		else
			alphaMod = 0.0f;

		TransferObj* transferObj = new TransferObj(transferFunction, ranges, color, alphaMod, chan, buff, numElem);
		dataQueue->writeMessage("transferUpdate", (void*)transferObj);
	}
}

std::string MexTransferUpdate::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (1 > nrhs || 2 < nrhs)
		return "This is not the right number of input arguments for transferUpdate!";

	return "";
}

void MexTransferUpdate::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	//inArgs.push_back("arrowFaces");
	//inArgs.push_back("arrowVerts");
}

void MexTransferUpdate::help(std::vector<std::string>& helpLines) const
{
	//helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	//helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
	//helpLines.push_back("\tArrowVerts -- This is a list of vertices that make up the polygon for each axis.");
}