#pragma once
#include "Renderer.h"
#include <DirectXMath.h>

class Material;

class MaterialParameters
{
public:
	friend class Renderer;
	friend class Material;
	virtual ~MaterialParameters();

	virtual DirectX::XMFLOAT4 getColor(){return DirectX::XMFLOAT4(0.0f,0.0f,0.0f,0.0f);}


private:
	// Disable creation of this without a renderer (force children to initialize properly)
	MaterialParameters(){};

protected:
	MaterialParameters(Renderer* rendererIn);

	void createResources(size_t size){renderer->createConstantBuffer(size, &constBuffer);}
	void releaseResources(){SAFE_RELEASE(constBuffer);}

	void setShaderResources(){renderer->setPixelShaderConsts(constBuffer);}
	void updateParams(){renderer->updateShaderParams(constMemory,constBuffer);}

	Renderer* renderer;
	ID3D11Buffer* constBuffer;

	void* constMemory;
};

class SingleColorParams: public MaterialParameters
{
public:
	SingleColorParams(Renderer* rendererIn);
	SingleColorParams(Renderer* rendererIn, Vec<float> colorIn, float alpha);

	void setColor(Vec<float> colorIn, float alpha);
	void setColorModifier(Vec<float> colorMod, float alphaMod);

	void setLightOn(bool on);

private:
	SingleColorParams() : MaterialParameters(NULL){};

	struct ColorBuffer
	{
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 colorModifier;
		DirectX::XMFLOAT4 lightOn;
	} colorBuffer;
};

class StaticVolumeParams : public MaterialParameters
{
public:
	StaticVolumeParams(Renderer* rendererIn);
	StaticVolumeParams(Renderer* rendererIn, int numChannelsIn);

	~StaticVolumeParams();

	//NOTE: This will reset all param data
	void resetChannels(int numChannelsIn);

	void setTransferFunction(int channel, Vec<float> transferFunction);
	void setRange(int channel, Vec<float> ranges);

	void setColor(int channel, Vec<float> color, float alphaMod);

	void setLightOn(bool on);
	void setAttenuationOn(bool on);
	void setGradientSampleDir(Vec<float> xDir, Vec<float> yDir, Vec<float> zDir);

private:
	static const Vec<float> colors[6];

	StaticVolumeParams() : MaterialParameters(NULL){};

	void allocMemory();
	void clearMemory();
	void setDefaultParams();

	struct StaticVolLayout
	{
		std::vector<DirectX::XMFLOAT4> transferFunctions;
		std::vector<DirectX::XMFLOAT4> ranges;
		std::vector<DirectX::XMFLOAT4> channelColors;
		DirectX::XMFLOAT4 gradientSampleDirection[3];
		DirectX::XMFLOAT4 lightOn;

		static size_t sizeOf(int numChannels)
		{
			size_t size = 0;

			size += numChannels * sizeof(DirectX::XMFLOAT4);
			size += numChannels * sizeof(DirectX::XMFLOAT4);
			size += numChannels * sizeof(DirectX::XMFLOAT4);
			size += 3 * sizeof(DirectX::XMFLOAT4);
			size += sizeof(DirectX::XMFLOAT4);

			return size;
		}
	};

	int numChannels;
};