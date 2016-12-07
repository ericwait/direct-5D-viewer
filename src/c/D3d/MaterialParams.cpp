#include "MaterialParams.h"

// Base material parameter functions
MaterialParameters::MaterialParameters(Renderer* rendererIn)
	: renderer(rendererIn), constMemory(NULL), constBuffer(NULL)
{}

MaterialParameters::~MaterialParameters()
{
	releaseResources();
}



SingleColorParams::SingleColorParams(Renderer* rendererIn)
	: MaterialParameters(rendererIn)
{
	constMemory = &colorBuffer;

	colorBuffer.color = DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	colorBuffer.colorModifier = DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	colorBuffer.lightOn = DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f);

	createResources(sizeof(colorBuffer));
}

// Material parameters for a single color polygon
SingleColorParams::SingleColorParams(Renderer* rendererIn, Vec<float> colorIn, float alpha)
	: MaterialParameters(rendererIn)
{
	constMemory = &colorBuffer;

	colorBuffer.color = DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z,alpha);
	colorBuffer.colorModifier = DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	colorBuffer.lightOn = DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f);

	createResources(sizeof(colorBuffer));
}

void SingleColorParams::setColor(Vec<float> colorIn, float alpha)
{
	colorBuffer.color = DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z, alpha);
}

void SingleColorParams::setColorModifier(Vec<float> colorMod, float alphaMod)
{
	colorBuffer.colorModifier = DirectX::XMFLOAT4(colorMod.x,colorMod.y,colorMod.z,alphaMod);
}

void SingleColorParams::setLightOn(bool on)
{
	colorBuffer.lightOn = DirectX::XMFLOAT4(on, on, on, on);
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

StaticVolumeParams::~StaticVolumeParams()
{
	clearMemory();
}

void StaticVolumeParams::resetChannels(int numChannelsIn)
{
	numChannels = numChannelsIn;

	allocMemory();
	setDefaultParams();

	releaseResources();
	createResources(StaticVolLayout::sizeOf(numChannels));
}

void StaticVolumeParams::setTransferFunction(int channel, Vec<float> transferFunction)
{
	const size_t tfnEntry = 4 * sizeof(float);

	//channel number of float4s before it
	const size_t memStart = channel * tfnEntry;

	memcpy((void*)(((unsigned char*)constMemory)+memStart), &(transferFunction), 3*sizeof(float));
}

void StaticVolumeParams::setRange(int channel, Vec<float> ranges)
{
	const size_t tfnEntry = 4 * sizeof(float);
	const size_t rangeEntry = 4 * sizeof(float);

	//channel number of float4 before it along with numChannel float4s behind that
	const size_t memStart = channel * rangeEntry + numChannels * tfnEntry;

	memcpy((void*)(((unsigned char*)constMemory)+memStart), &(ranges), 3*sizeof(float));
}

void StaticVolumeParams::setColor(int channel, Vec<float> color, float alphaMod)
{
	const size_t tfnEntry = 4 * sizeof(float);
	const size_t rangeEntry = 4 * sizeof(float);
	const size_t colorEntry = 4 * sizeof(float);

	//channel number of float4 before it along with 2 times numchannel float4s behind that
	const size_t memStart = channel * colorEntry + numChannels * (rangeEntry + tfnEntry);

	memcpy((void*)(((unsigned char*)constMemory)+memStart), &(color.x), 3*sizeof(float));
	memcpy((void*)(((unsigned char*)constMemory)+memStart+sizeof(float)*3), &(alphaMod), sizeof(float));
}

void StaticVolumeParams::setGradientSampleDir(Vec<float> xDir, Vec<float> yDir, Vec<float> zDir)
{
	const size_t tfnEntry = 4 * sizeof(float);
	const size_t rangeEntry = 4 * sizeof(float);
	const size_t colorEntry = 4 * sizeof(float);

	//3 times numchannel float4s behind
	size_t memStart = numChannels * (colorEntry + rangeEntry + tfnEntry);

	memcpy((void*)(((unsigned char*)constMemory)+memStart), &(xDir.x), 3*sizeof(float));
	memcpy((void*)(((unsigned char*)constMemory)+memStart+sizeof(float)*4), &(yDir.x), 3*sizeof(float));
	memcpy((void*)(((unsigned char*)constMemory)+memStart+sizeof(float)*8), &(zDir.x), 3*sizeof(float));
}

void StaticVolumeParams::allocMemory()
{
	clearMemory();

	constMemory = new unsigned char[StaticVolLayout::sizeOf(numChannels)];
}

void StaticVolumeParams::clearMemory()
{
	SAFE_DELETE_ARRAY(constMemory);
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

void StaticVolumeParams::setLightOn(bool on)
{
	const size_t tfnEntry = 4 * sizeof(float);
	const size_t rangeEntry = 4 * sizeof(float);
	const size_t colorEntry = 4 * sizeof(float);
	const size_t gradEntry = 4 * sizeof(float);

	float isOn = (on==true) ? (1.0f) : (0.0f);
	//3 float4s along with 3 times numchannel float4s behind
	const size_t memStart = 3 * gradEntry + numChannels * (colorEntry + rangeEntry + tfnEntry);

	memcpy((void*)(((unsigned char*)constMemory)+memStart), &isOn, sizeof(float));
}

void StaticVolumeParams::setAttenuationOn(bool on)
{
	const size_t tfnEntry = 4 * sizeof(float);
	const size_t rangeEntry = 4 * sizeof(float);
	const size_t colorEntry = 4 * sizeof(float);
	const size_t gradEntry = 4 * sizeof(float);
	const size_t lightSize = sizeof(float);

	float isOn = (on == true) ? (1.0f) : (0.0f);
	//2nd float plus 3 float4s along with 3 times numchannel float4s behind
	const size_t memStart = lightSize + 3 * gradEntry + numChannels * (colorEntry + rangeEntry + tfnEntry);

	memcpy((void*)(((unsigned char*)constMemory)+memStart), &isOn, sizeof(float));
}
