#include "MexCommand.h"
#include "MexFunctions.h"
#include "Global/Globals.h"
#include "Messages/Image.h"

#include <chrono>
#include <thread>

void MexCaptureWindow::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nlhs==0)
	{
		gMsgQueueToDirectX.writeMessage("CaptureWindow", NULL);
		return;
	}
	
	// This is when the caller would like to receive the image in memory instead of the file system
	Image* outImage = NULL;
	outImage = new Image(1, 1, Vec<size_t>(0, 0, 0));
	gMsgQueueToDirectX.writeMessage("CaptureWindow", outImage);

	while(!gMsgQueueToMex.doneLoading())
		std::this_thread::sleep_for(std::chrono::seconds(1));

	gMsgQueueToMex.clearLoadFlag();
	
	const Vec<size_t> dims = outImage->getDimensions();
	const mwSize DIMS[3] = {dims.y,dims.x,dims.z};

	plhs[0] = mxCreateNumericArray(3, DIMS, mxUINT8_CLASS, mxREAL);
	unsigned char* mIm = (unsigned char*)mxGetData(plhs[0]);
	unsigned char* screenIm = outImage->getPixels();

	size_t scanLine = ceil((float)dims.x/4.0f)*4;

	size_t matlabColorStride = dims.y*dims.x;

	for(size_t c = 0; c<3; ++c)
	{
		size_t bmpC = 3-c-1;
		size_t matC = c*matlabColorStride;

		for(size_t y = 0; y<dims.y; ++y)
		{
			size_t bmpY = dims.y-y-1;
			size_t matY = y;

			for(size_t x = 0; x<dims.x; ++x)
			{
				size_t bmpX = x*4;
				size_t matX = x;

				size_t bmpIdx = bmpX + bmpY*scanLine*4 + bmpC;
				size_t matIdx = matY + matX*dims.y + matC;

				mIm[matIdx] = screenIm[bmpIdx];
			}
		}
	}

	delete[] screenIm;
	delete outImage;
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
