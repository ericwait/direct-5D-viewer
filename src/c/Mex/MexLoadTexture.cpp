#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexLoadTexture::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	const mwSize* DIMS = mxGetDimensions(prhs[1]);

	// Dimensions 1-5
	Vec<size_t> dims = Vec<size_t>(DIMS[0], DIMS[1], DIMS[2]);
	int numChannels = 1;
	int numFrames = 1;


	if (numDims>3)
		numChannels = int(DIMS[3]);

	if (numDims > 4)
		numFrames = int(DIMS[4]);

	// ptr to image data
	unsigned char* image = (unsigned char*)mxGetData(prhs[1]);

	Image* img = new Image(numChannels, numFrames, dims);
	img->setPixels(image);

	if (nrhs > 2)
	{
		// Physical dimensions
		double* physDims = (double*)mxGetData(prhs[2]);
		img->setPhysicalDim(Vec<float>(float(physDims[1]), float(physDims[0]), float(physDims[2])));
	}

	if (nrhs > 3)
	{
		char buff[96];
		mxGetString(prhs[3], buff, 96);

		if (_strcmpi("original", buff) == 0)
			img->setTextureType(GraphicObjectTypes::OriginalVolume);
		else if (_strcmpi("processed", buff) == 0)
			img->setTextureType(GraphicObjectTypes::ProcessedVolume);
	}
	else
	{
		img->setTextureType(GraphicObjectTypes::OriginalVolume);
	}

	std::string s = "loadTexture";
	dataQueue->writeMessage(s, (void*)img);
	int x = 0;
	x = x + 30;
}

std::string MexLoadTexture::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (!mxIsClass(prhs[0], "uint8"))
		return "Image must be of uint8 class!";

	size_t numDims = mxGetNumberOfDimensions(prhs[0]);
	if (numDims<3)
		return "Image must have at least three dimensions!";

	return "";
}

void MexLoadTexture::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("arrowFaces");
}

void MexLoadTexture::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
}