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



SingleColorParams::SingleColorParams(Renderer* rendererIn)
	: MaterialParameters(rendererIn)
{
	addParam<DirectX::XMFLOAT4>("color", DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f), "Color all vertices");
	addParam<DirectX::XMFLOAT4>("colorModifier", DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f), "Multiplier on color");

	createResources();
}

// Material parameters for a single color polygon
SingleColorParams::SingleColorParams(Renderer* rendererIn, Vec<float> colorIn, float alpha)
	: MaterialParameters(rendererIn)
{
	addParam<DirectX::XMFLOAT4>("color", DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z,alpha), "Color all vertices");
	addParam<DirectX::XMFLOAT4>("colorModifier", DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f), "Multiplier on color");

	createResources();
}

void SingleColorParams::setColor(Vec<float> colorIn, float alpha)
{
	ref<DirectX::XMFLOAT4>("color") = DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z,alpha);
}

void SingleColorParams::setColorModifier(Vec<float> colorMod, float alphaMod)
{
	ref<DirectX::XMFLOAT4>("colorModifier") = DirectX::XMFLOAT4(colorMod.x,colorMod.y,colorMod.z,alphaMod);
}



// Material parameters for view-aligned triangle shader
const Vec<float> StaticVolumeParams::colors[6] = 
{
	Vec<float>(1.0f, 0.0f, 0.0f),
	Vec<float>(0.0f, 1.0f, 0.0f),
	Vec<float>(0.0f, 0.0f, 1.0f),
	Vec<float>(1.0f, 1.0f, 0.0f),
	Vec<float>(0.0f, 1.0f, 1.0f),
	Vec<float>(1.0f, 0.0f, 1.0f)
};

StaticVolumeParams::StaticVolumeParams(Renderer* rendererIn)
	: MaterialParameters(rendererIn)
{
	numChannels = 0;
}

StaticVolumeParams::StaticVolumeParams(Renderer* rendererIn, int numChannelsIn)
	: MaterialParameters(rendererIn)
{
	resetChannels(numChannelsIn);
}

void StaticVolumeParams::resetChannels(int numChannelsIn)
{
	numChannels = numChannelsIn;

	addParamArray<DirectX::XMFLOAT4>("transferFunctions", numChannels, "Array of transfer function params");
	addParamArray<DirectX::XMFLOAT4>("ranges", numChannels, "Max and min for transfer functions");
	addParamArray<DirectX::XMFLOAT4>("channelColors", numChannels, "Color values for each channel");
	addParamArray<DirectX::XMFLOAT4>("gradientDir", 3, "Local sample coordinate system for lighting normals");

	addParam<DirectX::XMFLOAT4>("flags", DirectX::XMFLOAT4(1.0,0.0,0.0,0.0), "Render flags: (lighting, attenuation, reserved, reserved)");

	setDefaultParams();

	releaseResources();
	createResources();
}

void StaticVolumeParams::setTransferFunction(int channel, Vec<float> transferFunction)
{
	ptr<DirectX::XMFLOAT4>("transferFunctions")[channel] = DirectX::XMFLOAT4(transferFunction.x, transferFunction.y, transferFunction.z, 0.0f);
}

void StaticVolumeParams::setRange(int channel, Vec<float> ranges)
{
	ptr<DirectX::XMFLOAT4>("ranges")[channel] = DirectX::XMFLOAT4(ranges.x, ranges.y, ranges.z, 0.0f);
}

void StaticVolumeParams::setColor(int channel, Vec<float> color, float alphaMod)
{
	ptr<DirectX::XMFLOAT4>("channelColors")[channel] = DirectX::XMFLOAT4(color.x, color.y, color.z, alphaMod);
}

void StaticVolumeParams::setGradientSampleDir(Vec<float> xDir, Vec<float> yDir, Vec<float> zDir)
{
	ptr<DirectX::XMFLOAT4>("gradientDir")[0] = DirectX::XMFLOAT4(xDir.x, xDir.y, xDir.z, 0.0f);
	ptr<DirectX::XMFLOAT4>("gradientDir")[1] = DirectX::XMFLOAT4(yDir.x, yDir.y, yDir.z, 0.0f);
	ptr<DirectX::XMFLOAT4>("gradientDir")[2] = DirectX::XMFLOAT4(zDir.x, zDir.y, zDir.z, 0.0f);
}

void StaticVolumeParams::setDefaultParams()
{
	for (int i = 0; i < numChannels; ++i)
	{
		setTransferFunction(i, Vec<float>(0.0f, 1.0f, 0.0f));
		setRange(i, Vec<float>(0.0f, 1.0f, 0.0f));
		setColor(i, colors[i % 6], 1.0f);
	}

	setAttenuationOn(false);
	setLightOn(false);
}
