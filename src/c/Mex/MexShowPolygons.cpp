#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexShowPolygons::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double mexOn = mxGetScalar(prhs[0]);
	double* on = new double;
	*on = mexOn;

	gMsgQueueToDirectX.writeMessage("ShowPolygons", (void*)on);
}

std::string MexShowPolygons::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for ShowPolygons!";

	return "";
}

void MexShowPolygons::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("on");
}

void MexShowPolygons::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will toggle the polygons on and off.");

	helpLines.push_back("\tOn - If this is set to one, then all of the polygons will be on. If this is 0, all of the polygons will be off.");
	helpLines.push_back("\t\tIf only a subset of the polygons should be displayed, use 'DisplayPolygons'.");
}