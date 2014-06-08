#pragma once
#include "Renderer.h"
#include <DirectXMath.h>

class Material
{
public:
	friend class Renderer;
	virtual ~Material();

	void setWireframe(bool wireframe);

protected:
	Material(){}
	Material(Renderer* rendererIn);

	virtual void updateParams() = 0;
	void setShader(const std::string& shaderFilename, const std::string& shaderFunction, const std::string& shaderParams);
	virtual void setShaderResources() = 0;
	
	Renderer* renderer;

	bool wireframe;
	bool cullBackFace;
	bool testDepth;

private:
	int shaderIdx;
};


class SingleColoredMaterial : public Material
{
public:
	SingleColoredMaterial(Renderer* renderer);
	SingleColoredMaterial(Renderer* renderer, Vec<float> colorIn);
	SingleColoredMaterial(Renderer* renderer, Vec<float> colorIn, float alpha);
	~SingleColoredMaterial();

	void setColor(Vec<float> colorIn, float alpha);
	void setColorModifier(Vec<float> colorMod, float alphaMod);
	void setShaderResources();

	void updateParams();

private:
	SingleColoredMaterial(){}
	
	struct ColorBuffer
	{
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 colorModifier;
	} colorBuffer;

	ID3D11Buffer* constBuffer;
};

class StaticVolumeTextureMaterial : public Material
{
public:
	StaticVolumeTextureMaterial(Renderer* rendererIn, Vec<size_t> dims, int numChannels, unsigned char* image);
	~StaticVolumeTextureMaterial();

	void setTransferFunction(int channel, Vec<float> transferFunction);
	void setRange(int channel, Vec<float> ranges);
	void setColor(int channel, Vec<float> color, float alphaMod);
	void setLightOn(bool on);
	void setGradientSampleDir(Vec<float> xDir, Vec<float> yDir, Vec<float> zDir);

	void updateParams();

private:
	StaticVolumeTextureMaterial();

	void setShaderResources();

	struct VolPixShaderConsts
	{
		std::vector<DirectX::XMFLOAT4> transferFunctions;
		std::vector<DirectX::XMFLOAT4> ranges;
		std::vector<DirectX::XMFLOAT4> channelColors;
		DirectX::XMFLOAT4 gradientSampleDirection[3];
		DirectX::XMFLOAT4 lightOn;

		size_t sizeOf()
		{
			size_t size = 0;
			size += transferFunctions.size() * sizeof(DirectX::XMFLOAT4);
			size += ranges.size() * sizeof(DirectX::XMFLOAT4);
			size += channelColors.size() * sizeof(DirectX::XMFLOAT4);
			size += 3 * sizeof(DirectX::XMFLOAT4);
			size += sizeof(DirectX::XMFLOAT4);

			return size;
		}
	}volPixShaderConsts;
	
	unsigned char* shaderConstMemory;
	int numChannels;
	Vec<size_t> dims;
	ID3D11Buffer* constBuffer;
	std::vector<ID3D11SamplerState*> samplerState;
	std::vector<ID3D11ShaderResourceView*> shaderResourceView;
};

