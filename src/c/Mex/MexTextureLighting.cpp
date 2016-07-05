#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"
#include "..\D3d\TextureLightingObj.h"

void MexTextureLighting::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (mxGetScalar(prhs[0]) > 0.0)
	{
		for (int i = 0; i < firstVolumeTextures.size(); ++i)
		{
			if (NULL != firstVolumeTextures[i])
			{
				TextureLightingObj* textureLightObj = new TextureLightingObj(true, i);
				gDataQueue->writeMessage("TextureLighting", (void*)textureLightObj);
				//firstVolumeTextures[i]->setLightOn(true);
			}
		}
	}
	else
	{
		for (int i = 0; i < firstVolumeTextures.size(); ++i)
		{
			if (NULL != firstVolumeTextures[i])
			{
				TextureLightingObj* textureLightObj = new TextureLightingObj(false, i);
				gDataQueue->writeMessage("TextureLighting", (void*)textureLightObj);
				//firstVolumeTextures[i]->setLightOn(false);
			}
		}
	}
}

std::string MexTextureLighting::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "not the right arguments for lightingUpdate!";

	return "";
}

void MexTextureLighting::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	//inArgs.push_back("arrowFaces");
}

void MexTextureLighting::help(std::vector<std::string>& helpLines) const
{
	//helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	//helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
}