#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/ViewMessages.h"

void MexMoveCamera::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double* deltas = (double*)mxGetData(prhs[0]);

	gMsgQueueToDirectX.pushMessage(new MessageMoveCamera(deltas));
	gMsgQueueToDirectX.pushMessage(new MessageUpdateRender());
}

std::string MexMoveCamera::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nrhs!=1)
		return "Move requires a 3-D vector of camera deltas.";

	if ( mxGetNumberOfElements(prhs[0]) != 3 )
		return "Move requires a 3-D vector of camera deltas.";

	return "";
}

void MexMoveCamera::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("deltas");
}

void MexMoveCamera::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will move the camera by the specified delta [x,y,z].");
	helpLines.push_back("\tDeltas -- specifies the amount to move the camera in [x,y,z].");
}