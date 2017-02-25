#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/ViewMessages.h"

void MexTransferFunction::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	char bufferType[256];
	mxGetString(prhs[1], bufferType, 256);

	for ( int i=GraphicObjectTypes::OriginalVolume; i < GraphicObjectTypes::NumGO; ++i )
	{
		size_t numChannels = mxGetNumberOfElements(prhs[0]);
		for ( int c=0; c < numChannels; ++c )
		{
			MessageUpdateTransferFcn* msg = new MessageUpdateTransferFcn((GraphicObjectTypes)i, c);

			mxArray* mxOn = mxGetField(prhs[0], c, "visible");
			mxArray* mxAlpha = mxGetField(prhs[0], c, "alphaMod");

			// Setup channel color and relative alpha
			float alpha = (float)mxGetScalar(mxAlpha);
			if ( mxGetScalar(mxOn) == 0.0 )
				alpha = 0.0f;

			mxArray* mxColor = mxGetField(prhs[0], c, "color");
			msg->setColor((double*)mxGetData(mxColor), alpha);

			// Setup min/max range
			mxArray* mxMin = mxGetField(prhs[0], c, "minVal");
			mxArray* mxMax = mxGetField(prhs[0], c, "maxVal");
			msg->setRange(Vec<float>((float)mxGetScalar(mxMin), (float)mxGetScalar(mxMax), 1.0f));

			// Setup transfer function parameters
			mxArray* mxA = mxGetField(prhs[0], c, "a");
			mxArray* mxB = mxGetField(prhs[0], c, "b");
			mxArray* mxC = mxGetField(prhs[0], c, "c");
			msg->setTransferFcn(Vec<float>((float)mxGetScalar(mxA), (float)mxGetScalar(mxB), (float)mxGetScalar(mxC)));

			gMsgQueueToDirectX.pushMessage(msg);
		}
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