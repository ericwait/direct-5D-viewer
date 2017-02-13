#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/ViewMessages.h"

void MexShowAllPolygons::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	bool polyOn = (mxGetScalar(prhs[0]) != 0.0);
	gMsgQueueToDirectX.pushMessage(new MessageShowObjectType(GraphicObjectTypes::Polygons, polyOn));
}

std::string MexShowAllPolygons::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for ShowAllPolygons!";

	return "";
}

void MexShowAllPolygons::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("on");
}

void MexShowAllPolygons::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will toggle the polygons on and off.");

	helpLines.push_back("\tOn - If this is set to one, then all of the polygons will be on. If this is 0, all of the polygons will be off.");
	helpLines.push_back("\t\tIf only a subset of the polygons should be displayed, use 'DisplayPolygons'.");
}