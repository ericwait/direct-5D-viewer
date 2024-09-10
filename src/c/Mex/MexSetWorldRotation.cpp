#include "MexCommand.h"
#include "Globals.h"
#include "Vec.h"

#include "ViewMessages.h"

void MexSetWorldRotation::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	Vec<double> axis((double*)mxGetData(prhs[0]));
	double angle = mxGetScalar(prhs[1]);

	gMsgQueueToDirectX.pushMessage(new MessageSetWorldRotation(axis, angle));
}

std::string MexSetWorldRotation::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nrhs!=2)
		return "Not the right arguments for set rotation!";

	size_t vecDim = mxGetNumberOfElements(prhs[0]);
	if(vecDim!=3)
		return "Axis needs to be a vector of three doubles representing (X,Y,Z).";

	return "";
}

void MexSetWorldRotation::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("rotationVector_xyz");
	inArgs.push_back("deltaAngle");
}

void MexSetWorldRotation::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will rotate the current view by the desired angle.");

	helpLines.push_back("\tRotationVector_xyz - This is the vector that will be rotated relative to the world coordinate system (around the current widget).");
	helpLines.push_back("\tDeltaAngle - This is the angle in degrees to rotate around the given vector.");
}
