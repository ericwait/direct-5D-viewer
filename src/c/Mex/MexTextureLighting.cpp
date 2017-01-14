#include "MexCommand.h"
#include "Global/Globals.h"
#include "D3d/TextureLightingObj.h"

void MexTextureLighting::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	for (int i = 0; i < 2; ++i)
	{
		TextureLightingObj* textureLightObj = new TextureLightingObj((mxGetScalar(prhs[0]) > 0.0), i);
		gMsgQueueToDirectX.writeMessage("TextureLighting", (void*)textureLightObj);
	}
}

std::string MexTextureLighting::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "Not the right arguments for lightingUpdate!";

	return "";
}

void MexTextureLighting::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("lightOn");
}

void MexTextureLighting::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This allows for the toggling the light on the texture.");

	helpLines.push_back("\tLightOn - This is a double, (0,1) where a zero means no light and a one means there is a directional light.");
}