#include "MexCommand.h"
#include "Global/Globals.h"
#include "Messages/TransferObj.h"

void MexTransferFunction::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	char bufferType[256];
	mxGetString(prhs[1], bufferType, 256);

	size_t numElem = mxGetNumberOfElements(prhs[0]);

	for (int chan = 0; chan < numElem; ++chan)
	{
		// TODO Pull these out.. eventually 
		Vec<float> transferFunction(0.0f, 0.0f, 0.0f);
		Vec<float> ranges;
		Vec<float> color;
		float alphaMod;

		mxArray* mxColorPt = mxGetField(prhs[0], chan, "color");
		double* mxColor = (double*)mxGetData(mxColorPt);
		color = Vec<float>((float)(mxColor[0]), (float)(mxColor[1]), (float)(mxColor[2]));

		mxArray* mxAPt = mxGetField(prhs[0], chan, "a");
		mxArray* mxBPt = mxGetField(prhs[0], chan, "b");
		mxArray* mxCPt = mxGetField(prhs[0], chan, "c");
		double a = mxGetScalar(mxAPt);
		double b = mxGetScalar(mxBPt);
		double c = mxGetScalar(mxCPt);
		transferFunction = Vec<float>((float)a, (float)b, (float)c);

		mxArray* mxMin = mxGetField(prhs[0], chan, "minVal");
		mxArray* mxMax = mxGetField(prhs[0], chan, "maxVal");
		ranges = Vec<float>((float)mxGetScalar(mxMin), (float)mxGetScalar(mxMax), 1.0f);

		mxArray* mxAlphaPt = mxGetField(prhs[0], chan, "alphaMod");
		mxArray* mxOnPt = mxGetField(prhs[0], chan, "visible");
		if (mxGetScalar(mxOnPt) != 0)
			alphaMod = (float)mxGetScalar(mxAlphaPt);
		else
			alphaMod = 0.0f;

		TransferObj* transferObj = new TransferObj(transferFunction, ranges, color, alphaMod, chan, bufferType, numElem);
		gMsgQueueToDirectX.writeMessage("TransferFunction", (void*)transferObj);
	}
}

std::string MexTransferFunction::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (2 != nrhs)
		return "This is not the right number of input arguments for TransferFunction!";

	return "";
}

void MexTransferFunction::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("TransferFunctionStruct");
	inArgs.push_back("BufferType");
}

void MexTransferFunction::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("these are the values for the transfer function.  The transfer function is the quadratic mapping of input intensities to output (viewed) intensities per channel and buffer.");

	helpLines.push_back("\tTransferFunctionStruct -- this is a channel number sized array of structures with the fields: color, a, b, c, minVal, maxVal, aphlaMod, visible.");
	helpLines.push_back("\t\tColor is a array of three values (r,g,b) between [0,1] that represent the color of a channel.");
	helpLines.push_back("\t\tA, B, & C represent the quadratic equation that the transfer function will apply (a*x^2 + b*x + c).");
	helpLines.push_back("\t\tMin and Max values are the clipping thresholds that are used to ensure that the quadratic equation is monotonically increasing between them.");
	helpLines.push_back("\t\tAlphaMod is the alpha multiplier to change the transparency of the channel.");
	helpLines.push_back("\t\tVisible is a logical flag that represents whether or not a channel is visible.");

	helpLines.push_back("\tBufferType is the buffer that you would like to apply these setting to.  The possible values are 'original' and 'processed'. The correspond to the first and second texture buffers.");
}