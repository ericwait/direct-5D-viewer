#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/ViewMessages.h"

void MexTextureAttenuation::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	bool attenuateOn = (mxGetScalar(prhs[0]) != 0.0);

	for ( int i=GraphicObjectTypes::OriginalVolume; i < GraphicObjectTypes::VTend; ++i )
		gMsgQueueToDirectX.pushMessage(new MessageSetTextureAttenuation((GraphicObjectTypes)i, attenuateOn));
}

std::string MexTextureAttenuation::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "not the right arguments for attenuationUpdate!";

	return "";
}

void MexTextureAttenuation::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("on");
}

void MexTextureAttenuation::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This allows for the toggling the attenuation of intensities on the texture.");

	helpLines.push_back("\tOn - This is a double, [0,1] where a zero means no attenuation and a one means there is a attenuation applied to the texture.");
}