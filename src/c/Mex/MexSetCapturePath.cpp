#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexSetCapturePath::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	char filePath[512];
	char fileName[255];
	mxGetString(prhs[0], filePath, 512);
	mxGetString(prhs[1], fileName, 255);

	gRenderer->setCaptureFilePath(filePath);
	gRenderer->setCaptureFileName(fileName);
}

std::string MexSetCapturePath::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 2)
		return "Not the right arguments for setCapturePath!";

	return "";
}

void MexSetCapturePath::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("filePath");
	inArgs.push_back("filePrefix");
}

void MexSetCapturePath::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will set the path and file name prefix for all captured frames.");

	helpLines.push_back("\tFilePath - This is the path were the bmp frame captures will be saved.");
	helpLines.push_back("\tFilePrefix - This is the prefix to the files that will be saved to the FilePath.");
	helpLines.push_back("\t\tFiles will be saved with the name prefix%04d.bmp, where the numbering starts at one and increases until the program is closed.");
}