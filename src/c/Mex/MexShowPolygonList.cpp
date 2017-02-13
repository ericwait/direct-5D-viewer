#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/ViewMessages.h"

#include <set>

void MexShowPolygonList::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	mwSize numPolys = mxGetNumberOfElements(prhs[0]);
	double* polyList = (double*)mxGetData(prhs[0]);

	MessageShowPolys* polyMsg = new MessageShowPolys(true);
	for ( mwSize i = 0; i < numPolys; ++i )
		polyMsg->setPoly((int) polyList[i]);

	gMsgQueueToDirectX.pushMessage(polyMsg);
}

std::string MexShowPolygonList::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for ShowPolygonList!";

	return "";
}

void MexShowPolygonList::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("polygonIndices");
}

void MexShowPolygonList::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will show only the listed polygons.");

	helpLines.push_back("\tPolygonIdxList - This is a list of polygon indices that correspond to the index field passed to the AddPolygon command.");
	helpLines.push_back("\t\tOnly the polygons in this list will be displayed.");
}