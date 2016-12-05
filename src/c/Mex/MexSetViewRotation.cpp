#include "MexCommand.h"
#include "MexFunctions.h"
#include "Global/Globals.h"
#include "Global/Vec.h"

void MexSetViewRotation::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double* vec = (double*)mxGetData(prhs[0]);
	
	double* vals = new double[4];
	vals[0] = vec[0];
	vals[1] = vec[1];
	vals[2] = vec[2];
	vals[3] = mxGetScalar(prhs[1]);

	gMsgQueueToDirectX.writeMessage("SetRotation", (void*)vals);
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
