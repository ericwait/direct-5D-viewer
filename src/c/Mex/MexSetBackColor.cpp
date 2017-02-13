#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/ViewMessages.h"

void MexSetBackgroundColor::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double* colorData = (double*) mxGetData(prhs[0]);
	Vec<float> color(colorData);

	gMsgQueueToDirectX.pushMessage(new MessageSetBackgroundColor(color));
}

std::string MexSetBackgroundColor::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "There must be one input consisting of three doubles between [0,1]!";

	if ( mxGetNumberOfElements(prhs[0]) != 3 )
		return "There must be one input consisting of an rgb color with elements in [0,1]!";

	double* colorData = (double*)mxGetData(prhs[0]);
	for ( int i=0; i < 3; ++i )
	{
		if ( colorData[i] < 0.0 || colorData[i] > 1.0 )
			return "Rgb color elements must be in [0,1]!";
	}

	return "";
}

void MexSetBackgroundColor::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("color");
}

void MexSetBackgroundColor::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will set the color of the background.");

	helpLines.push_back("\tColor - This should be three doubles between [0,1] that represent (r,g,b) of the desired background color.");
}