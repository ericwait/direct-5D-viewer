#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"
#include "..\Global\Vec.h"

void MexSetRotation::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double mexDeltaX = mxGetScalar(prhs[0]);
	double mexDeltaY = mxGetScalar(prhs[1]);
	double mexDeltaZ = mxGetScalar(prhs[2]);
	Vec<double>* delta = new Vec<double>;

	*delta = Vec<double>(mexDeltaX, mexDeltaY, mexDeltaZ);

	gMsgQueueToDirectX.writeMessage("SetRotation", (void*)delta);
}

std::string MexSetRotation::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nrhs!=3)
		return "Not the right arguments for set rotation!";

	return "";
}

void MexSetRotation::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("deltaXangle");
	inArgs.push_back("deltaYangle");
	inArgs.push_back("deltaZangle");
}

void MexSetRotation::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will rotate the current view by the desired angle.");

	helpLines.push_back("\tDeltaXangle - This will rotate around the X axis by the given angle.");
	helpLines.push_back("\tDeltaYangle - This will rotate around the Y axis by the given angle.");
	helpLines.push_back("\tDeltaZangle - This will rotate around the Z axis by the given angle.");
}
