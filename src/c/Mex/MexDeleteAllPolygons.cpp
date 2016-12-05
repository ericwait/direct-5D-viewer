#include "MexCommand.h"
#include "MexFunctions.h"
#include "Global/Globals.h"

void MexDeleteAllPolygons::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.writeMessage("deleteAllPolygons", (void*)NULL);
}

std::string MexDeleteAllPolygons::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexDeleteAllPolygons::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
}

void MexDeleteAllPolygons::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will remove all of the polygons.");
}