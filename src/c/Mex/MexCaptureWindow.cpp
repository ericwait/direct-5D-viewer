#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/AnimMessages.h"

#include <chrono>
#include <thread>

void MexCaptureWindow::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nlhs==0)
	{
		gMsgQueueToDirectX.pushMessage(new MessageCaptureWindow());

		return;
	}
	
	// This is when the caller would like to receive the image in memory instead of the file system
	MessageCaptureWindow::BMPData outData;
	gMsgQueueToDirectX.pushMessage(new MessageCaptureWindow(&outData), true);

	// TODO: Remove this!
	gMsgQueueToMex.clearLoadFlag();
	
	const mwSize DIMS[3] = {outData.height, outData.width, 3};

	plhs[0] = mxCreateNumericArray(3, DIMS, mxUINT8_CLASS, mxREAL);
	unsigned char* outIm = (unsigned char*)mxGetData(plhs[0]);

	//size_t pad = ((outData.width & 0x3) > 0);
	//size_t padWidth = ((outData.width >> 2) + pad) << 2;

	//size_t matlabColorStride = outData.height * outData.width;

	//for(size_t c = 0; c < 3; ++c)
	//{
	//	size_t bmpC = 3 - c - 1;
	//	size_t matC = c * matlabColorStride;

	//	for(size_t y = 0; y < outData.height; ++y)
	//	{
	//		size_t bmpY = outData.height - y - 1;
	//		size_t matY = y;

	//		for(size_t x = 0; x < outData.width; ++x)
	//		{
	//			size_t bmpX = x * 4;
	//			size_t matX = x;

	//			size_t bmpIdx = bmpX + bmpY * padWidth * 4 + bmpC;
	//			size_t matIdx = matY + matX * outData.height + matC;

	//			outIm[matIdx] = outData.data[bmpIdx];
	//		}
	//	}
	//}

	for ( size_t c=0; c < 3; ++c )
	{
		for ( size_t y = 0; y < outData.height; ++y )
		{
			for ( size_t x = 0; x < outData.width; ++x )
			{
				size_t matIdx = y + x * outData.height + c * outData.height * outData.width;
				size_t texIdx = c + 4 * (x + y * outData.width);
				outIm[matIdx] = outData.data[texIdx];
			}
		}
	}

	delete[] outData.data;
}

std::string MexCaptureWindow::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	return "";
}

void MexCaptureWindow::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	outArgs.push_back("ImageOut");
}

void MexCaptureWindow::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will capture the current view to either the current directory or the directory specified in 'SetCapturePath'.");

	helpLines.push_back("\tImageOut -- if an output is specified, the capture will be returned instead of written to the file system.");
}
