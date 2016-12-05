#include "MexCommand.h"
#include "MexFunctions.h"
#include "Global/Globals.h"

void MexRemovePolygon::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double d = mxGetScalar(prhs[0]);
	int* label = new int;
	*label = (int)d;
	gMsgQueueToDirectX.writeMessage("removePolygon", (void*)label);
}

std::string MexRemovePolygon::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexRemovePolygon::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
}

void MexRemovePolygon::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This removes all polygons from the directX viewer");
}