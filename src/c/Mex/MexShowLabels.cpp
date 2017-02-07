#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/ViewMessages.h"

void MexShowLabels::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	bool labelOn = (mxGetScalar(prhs[0]) != 0.0);
	gMsgQueueToDirectX.pushMessage(new MessageShowLabels(labelOn));
	gMsgQueueToDirectX.pushMessage(new MessageUpdateRender());
}

std::string MexShowLabels::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for showLabels!";

	return "";
}

void MexShowLabels::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("on");
}

void MexShowLabels::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will toggle the polygon labels on and off.");

	helpLines.push_back("\tOn - If this is set to one, then all of the polygon labels will be on. If this is 0, all of the polygon labels will be off.");
}