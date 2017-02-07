#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/AnimMessages.h"

void MexPlay::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	bool playMovie = (mxGetScalar(prhs[0]) != 0.0);
	gMsgQueueToDirectX.pushMessage(new MessageSetPlayMovie(playMovie));
}

std::string MexPlay::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for play!";

	return "";
}

void MexPlay::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("playOn");
}

void MexPlay::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will toggle the automatic playing of the movie.");

	helpLines.push_back("\tPlayOn - This is a double that is either a zero or a one, where a zero indicates not playing and a one indicates playing.");
}