#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/LoadMessages.h"

void MexRemovePolygon::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	int index = (int)mxGetScalar(prhs[0]);
	gMsgQueueToDirectX.pushMessage(new MessageDeletePoly(index));
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