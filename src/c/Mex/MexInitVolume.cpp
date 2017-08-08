#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/LoadMessages.h"

void MexInitVolume::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	// check the message queue for an error message before continuing
	if ( gMsgQueueToMex.hasError() )
		return;

	mwSize numDims = mxGetNumberOfElements(prhs[0]);
	const double* imDims = (double*)mxGetData(prhs[0]);

	// This will down-convert the doubles to size_t
	Vec<size_t> dims(imDims);

	int numFrames = 1;
	int numChannels = 1;
	if ( numDims > 3 )
		numChannels = (int) imDims[3];

	if ( numDims > 4 )
		numFrames = (int)imDims[4];

	// Set up voxel dimensions if provided
	Vec<float> physVoxel(1.0f, 1.0f, 1.0f);
	if ( nrhs > 1 )
	{
		// TODO: Should look over this and verify it's right for x-y anisotropy!
		double* physData = (double*)mxGetData(prhs[1]);
		physVoxel = Vec<float>(physData[0], physData[1], physData[2]);
	}

	// Compute total volume size in physical units
	Vec<float> physSize = Vec<float>(dims) * physVoxel;

	const bool columnMajor = true;
	gMsgQueueToDirectX.pushMessage(new MessageInitVolume(numFrames, numChannels, dims, physSize, columnMajor));
}

std::string MexInitVolume::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if ( nrhs < 1 )
		return "Function requires at least the dimesnions of the volume data!";

	if ( mxGetNumberOfElements(prhs[0]) < 3 )
		return "ImageDims must be at least 3-elemnts to specify the dimensions of static 3-D data!";

	if ( mxGetNumberOfElements(prhs[0]) > 5 )
		return "ImageDims has too many elements!";

	if ( nrhs < 2 )
		return "";

	// Handle secondary (optional) argument
	if ( nrhs > 2 )
		return "Too many input arguments!";

	if ( mxGetNumberOfElements(prhs[1]) != 3 )
		return "PhysicalUnits must be a 3-element vector indicating physical size of voxel dimensions!";

	return "";
}

void MexInitVolume::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("ImageDims");
	inArgs.push_back("PhysicalUnits");
}

void MexInitVolume::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("this will initialize the viewer with data necessary for loading volume textures.");

	helpLines.push_back("\tImage -- This should be a vector up to five elements indicating the volume size in the order (y,x,z,channel,time).");
	helpLines.push_back("\tPhysicalUnits -- A 3-element vector representing physical units of each voxel dimension (e.g. in microns/voxel).");
}
