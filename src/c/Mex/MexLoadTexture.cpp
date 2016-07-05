#include "MexCommand.h"
#include "..\Global\Globals.h"
#include "..\Messages\Image.h"

void MexLoadTexture::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	const mwSize* DIMS = mxGetDimensions(prhs[0]);

	// Dimensions 1-5
	Vec<size_t> dims = Vec<size_t>(DIMS[0], DIMS[1], DIMS[2]);
	int numChannels = 1;
	int numFrames = 1;
	
	size_t numDims = mxGetNumberOfDimensions(prhs[0]);
	if (numDims>3)
		numChannels = int(DIMS[3]);

	if (numDims > 4)
		numFrames = int(DIMS[4]);

	// ptr to image data
	unsigned char* image = (unsigned char*)mxGetData(prhs[0]);

	Image* img = new Image(numChannels, numFrames, dims);
	img->setPixels(image);

	if (nrhs > 2)
	{
		// Physical dimensions
		double* physDims = (double*)mxGetData(prhs[1]);
		img->setPhysicalDim(Vec<float>(float(physDims[1]), float(physDims[0]), float(physDims[2])));
	}

	if (nrhs > 2)
	{
		char buff[96];
		mxGetString(prhs[2], buff, 96);

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
	gDataQueue->writeMessage(s, (void*)img);
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
	inArgs.push_back("Image");
	inArgs.push_back("AnisotropicVoxelDimension");
	inArgs.push_back("BufferType");
}

void MexLoadTexture::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("this will load the image into texture buffers for display in the D3d viewer.");

	helpLines.push_back("\tImage -- This should be an matrix up to five dimensions in the order (y,x,z,channel,time).");
	helpLines.push_back("\tAnisotropicVoxelDimension -- This is the dimensions of a voxel (3D pixel) in microns. This will ensure that the displayed voxels are isotropic and that the scale bar displays properly.");
	helpLines.push_back("\tBufferType -- this can either be 'original' or 'processed' and corresponds to the first and second texture buffer available to load images into.");
}