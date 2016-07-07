#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"
#include "..\D3d\TextureLightingObj.h"

void MexTextureAttenuation::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	bool* on = new bool;
	*on = mxGetScalar(prhs[0])>0.0;
	gMsgQueueToDirectX.writeMessage("textureAttenUpdate", (void*)on);
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