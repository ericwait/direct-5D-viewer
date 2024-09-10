#include "MaterialParams.h"

// Base material parameter functions
MaterialParameters::MaterialParameters(Renderer* rendererIn)
	: renderer(rendererIn), constBuffer(NULL)
{
	// Default to lighting on, attenuation off
	addParam<DirectX::XMFLOAT4>("flags", DirectX::XMFLOAT4(1.0,0.0,0.0,0.0), "Render flags: (lighting, attenuation, reserved, reserved)");
}

void MaterialParameters::clearParams()
{
	constMemory.clear();
	paramHash.clear();
}

MaterialParameters::~MaterialParameters()
{
	clearParams();
	releaseResources();
}

void MaterialParameters::setLightOn(bool on)
{
	ref<DirectX::XMFLOAT4>("flags").x = ((on) ? 1.0f : 0.0f);
}

void MaterialParameters::setAttenuationOn(bool on)
{
	ref<DirectX::XMFLOAT4>("flags").y = ((on) ? 1.0f : 0.0f);
}



// Just has the default flags material parameters
DefaultParams::DefaultParams(Renderer* rendererIn)
	: MaterialParameters(rendererIn)
{}



// Material parameters for a single color polygon
PolygonParams::PolygonParams(Renderer* rendererIn)
	: MaterialParameters(rendererIn)
{
	addParam<DirectX::XMFLOAT4>("colorModifier", DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f), "Multiplier on vertex color");
}

void PolygonParams::setColorModifier(Vec<float> colorMod, float alphaMod)
{
	ref<DirectX::XMFLOAT4>("colorModifier") = DirectX::XMFLOAT4(colorMod.x,colorMod.y,colorMod.z,alphaMod);
}


// Material parameters for any volume renderer
const Vec<float> VolumeParams::defaultColors[6] = 
{
	Vec<float>(1.0f, 0.0f, 0.0f),
	Vec<float>(0.0f, 1.0f, 0.0f),
	Vec<float>(0.0f, 0.0f, 1.0f),
	Vec<float>(1.0f, 1.0f, 0.0f),
	Vec<float>(0.0f, 1.0f, 1.0f),
	Vec<float>(1.0f, 0.0f, 1.0f)
};

VolumeParams::VolumeParams(Renderer* rendererIn)
	: MaterialParameters(rendererIn), numChannels(0)
{}

VolumeParams::VolumeParams(Renderer* rendererIn, int numChannelsIn)
	: MaterialParameters(rendererIn), numChannels(0)
{
	resetChannels(numChannelsIn);
}

void VolumeParams::resetChannels(int numChannelsIn)
{
	numChannels = numChannelsIn;

	addParamArray<DirectX::XMFLOAT4>("transferFunctions", numChannels, "Array of transfer function params");
	addParamArray<DirectX::XMFLOAT4>("ranges", numChannels, "Max and min for transfer functions");
	addParamArray<DirectX::XMFLOAT4>("channelColors", numChannels, "Color values for each channel");

	setDefaultParams();
}

void VolumeParams::setDefaultParams()
{
	for (int i = 0; i < numChannels; ++i)
	{
		setTransferFunction(i, Vec<float>(0.0f, 1.0f, 0.0f));
		setRange(i, Vec<float>(0.0f, 1.0f, 0.0f));
		setColor(i, defaultColors[i % 6], 1.0f);
	}

	setAttenuationOn(false);
	setLightOn(false);
}

void VolumeParams::setTransferFunction(int channel, Vec<float> transferFunction)
{
	ptr<DirectX::XMFLOAT4>("transferFunctions")[channel] = DirectX::XMFLOAT4(transferFunction.x, transferFunction.y, transferFunction.z, 0.0f);
}

void VolumeParams::setRange(int channel, Vec<float> ranges)
{
	ptr<DirectX::XMFLOAT4>("ranges")[channel] = DirectX::XMFLOAT4(ranges.x, ranges.y, ranges.z, 0.0f);
}

void VolumeParams::setColor(int channel, Vec<float> color, float alphaMod)
{
	ptr<DirectX::XMFLOAT4>("channelColors")[channel] = DirectX::XMFLOAT4(color.x, color.y, color.z, alphaMod);
}



// Additional material parameters for view-aligned triangle renderer
StaticVolumeParams::StaticVolumeParams(Renderer* rendererIn, int numChannelsIn)
	: VolumeParams(rendererIn,numChannelsIn)
{
	addParamArray<DirectX::XMFLOAT4>("gradientDir", 3, "Local sample coordinate system for lighting normals.");
}

StaticVolumeParams::StaticVolumeParams(Renderer* rendererIn)
	: StaticVolumeParams(rendererIn,0)
{}

void StaticVolumeParams::setGradientSampleDir(Vec<float> xDir, Vec<float> yDir, Vec<float> zDir)
{
	ptr<DirectX::XMFLOAT4>("gradientDir")[0] = DirectX::XMFLOAT4(xDir.x, xDir.y, xDir.z, 0.0f);
	ptr<DirectX::XMFLOAT4>("gradientDir")[1] = DirectX::XMFLOAT4(yDir.x, yDir.y, yDir.z, 0.0f);
	ptr<DirectX::XMFLOAT4>("gradientDir")[2] = DirectX::XMFLOAT4(zDir.x, zDir.y, zDir.z, 0.0f);
}
