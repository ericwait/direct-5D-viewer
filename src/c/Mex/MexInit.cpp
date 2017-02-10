#include "MexGlobals.h"
#include "MexCommand.h"
#include "Widget.h"
#include "Global/Globals.h"
#include "Messages/Threads.h"

void MexInit::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	char buff[512];
	mxGetString(prhs[0], buff, 512);
	std::string rootDir = buff;
	startThread(rootDir);

	mexAtExit(exitFunc);

	// TODO: Should probably figure out what we want to do about the widget
	//       For now data is compiled in and loaded right after renderer init
	//loadWidget(prhs);
}

std::string MexInit::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Incorrect number of inputs!";

	if ( !mxIsChar(prhs[0]) )
		return "pathStr must be a string specifying the path to the mex.dll!";

	return "";
}

void MexInit::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("pathStr");
}

void MexInit::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This initializes the D3d viewing window.");
	
	helpLines.push_back("\tPathStr -- This is the path where the render will find the shader files and is typically in the same directory as the mex dll");
}