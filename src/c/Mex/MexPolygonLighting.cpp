#include "MexCommand.h"
#include "MexFunctions.h"
#include "Global/Globals.h"

void MexPolygonLighting::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double onD = mxGetScalar(prhs[0]);
	double* onD2 = new double;
	*onD2 = onD;
	gMsgQueueToDirectX.writeMessage("PolygonLighting", (void*)onD2);
}

std::string MexPolygonLighting::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for PolygonLighting!";

	return "";
}

void MexPolygonLighting::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("lightOn");
}

void MexPolygonLighting::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This allows for the toggling the light on the polygons.");

	helpLines.push_back("\tLightOn - This is a double, (0,1) where a zero means no light and a one means there is a directional light.");
}