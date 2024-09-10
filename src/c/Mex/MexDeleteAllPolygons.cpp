#include "MexCommand.h"
#include "Globals.h"

#include "LoadMessages.h"

void MexDeleteAllPolygons::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	gMsgQueueToDirectX.pushMessage(new MessageDeleteAllPolys());
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