#include "MexCommand.h"
#include "MexFunctions.h"
#include "Global/Globals.h"

void MexPoll::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	std::vector<RtnMessage> curMsgs = gMsgQueueToMex.flushQueue();

	const char* fields[] = {"command", "message", "val", "array"};
	plhs[0] = mxCreateStructMatrix(curMsgs.size(), 1, 4, fields);

	for(int i = 0; i < curMsgs.size(); ++i)
	{
		RtnMessage m = curMsgs[i];
		mxArray* cmd = mxCreateString(m.command.c_str());
		mxArray* msg = mxCreateString(m.message.c_str());
		mxArray* val = mxCreateDoubleScalar(m.val1);
		mxArray* aray;
		if(m.aray!=NULL)
		{
			unsigned int numRows = (unsigned int)m.val1;
			unsigned int numCols = (unsigned int)m.val2;

			size_t dims[3];
			dims[0] = numRows;
			dims[1] = numCols;
			dims[2] = 3;//TODO This needs to change for any messages that are not 3D

			aray = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
			void* arayPtr = mxGetData(aray);
			memcpy(arayPtr, m.aray, sizeof(unsigned char)*numRows*numCols*3);

			delete[] m.aray;
		} else
			aray = mxCreateDoubleMatrix(0, 0, mxREAL);

		mxSetField(plhs[0], i, fields[0], cmd);
		mxSetField(plhs[0], i, fields[1], msg);
		mxSetField(plhs[0], i, fields[2], val);
		mxSetField(plhs[0], i, fields[3], aray);
	}
}

std::string MexPoll::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nlhs < 1)
		return "Incorrect number of outputs!";

	return "";
}

void MexPoll::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	outArgs.push_back("MessageArray");
}

void MexPoll::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("");
}