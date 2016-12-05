#include "MexCommand.h"
#include "MexFunctions.h"
#include "Global/Globals.h"

void MexPeelUpdate::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	float* x = new float;
	*x = (float)mxGetScalar(prhs[0]);

	gMsgQueueToDirectX.writeMessage("peelUpdate", (void*)x);
}

std::string MexPeelUpdate::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nrhs!=1)
		return "Not the right arguments for peelUpdate!";

	return "";
}

void MexPeelUpdate::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("PeelSize");
}

void MexPeelUpdate::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will set the percent of the volume to render before rendering the polygons in the same area.");

	helpLines.push_back("\tPeelSize -- This is the percentage of the volume to render before rendering polygons in the same space.");
}