#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexShowPolygonList::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double* polyList = (double*)mxGetData(prhs[0]);
	size_t numPolygons = mxGetNumberOfElements(prhs[0]);

	std::set<int>* polygonSet = new std::set<int>;
	for(size_t i = 0; i<numPolygons; ++i)
		polygonSet->insert((int)(polyList[i]));

	gMsgQueueToDirectX.writeMessage("DisplayPolygons", (void*)polygonSet);
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