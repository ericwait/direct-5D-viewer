#include "MexCommand.h"
#include "Global/Globals.h"

#include "Global/Vec.h"

#include "Messages/ViewMessages.h"

void MexSetViewRotation::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	Vec<double> axis((double*)mxGetData(prhs[0]));
	double angle = mxGetScalar(prhs[1]);

	gMsgQueueToDirectX.pushMessage(new MessageSetViewRotation(axis, angle));
}

std::string MexSetViewRotation::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nrhs!=2)
		return "Not the right arguments for set rotation!";

	size_t vecDim = mxGetNumberOfElements(prhs[0]);
	if(vecDim!=3)
		return "Axis needs to be a vector of three doubles representing (X,Y,Z).";

	return "";
}

void MexSetViewRotation::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("rotationVector_xyz");
	inArgs.push_back("deltaAngle");
}

void MexSetViewRotation::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will rotate the current view by the desired angle.");

	helpLines.push_back("\tRotationVector_xyz - This is the vector that will be rotated relative to the view aligned coordinate system.");
	helpLines.push_back("\tDeltaAngle - This is the angle in degrees to rotate around the given vector.");
}
