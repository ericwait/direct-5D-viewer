#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/AnimMessages.h"

void MexSpin::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	bool spin = (mxGetScalar(prhs[0]) != 0.0);
	gMsgQueueToDirectX.pushMessage(new MessageSetSpinning(spin));
}

std::string MexSpin::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for rotate!";

	return "";
}

void MexSpin::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("on");
}

void MexSpin::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will start the volume rotating about the view aligned y axis.");

	helpLines.push_back("\tOn - If this is set to one, then the volume will rotate. If this is 0, the volume will not rotate.");
}