////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Renderer.h"
#include <DirectXMath.h>

class Material
{
public:
	friend class Renderer;
	virtual ~Material();

	void setWireframe(bool wireframe);
	virtual DirectX::XMFLOAT4 getColor(){return DirectX::XMFLOAT4(0.0f,0.0f,0.0f,0.0f);}

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
	
	DirectX::XMFLOAT4 getColor(){return colorBuffer.color;}

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
	StaticVolumeTextureMaterial(Renderer* rendererIn, Vec<size_t> dims, int numChannels, unsigned char* image,
		unsigned char* shaderConstMemoryIn=NULL);
	~StaticVolumeTextureMaterial();

	static const Vec<float> colors[6];

	void setTransferFunction(int channel, Vec<float> transferFunction);
	void setRange(int channel, Vec<float> ranges);
	void setColor(int channel, Vec<float> color, float alphaMod);
	void setLightOn(bool on);
	void setAttenuationOn(bool on);
	void setGradientSampleDir(Vec<float> xDir, Vec<float> yDir, Vec<float> zDir);
	unsigned char* getShaderConstMemory(){return shaderConstMemory;}

	bool isLightOn(){return lightingOn;}

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
	bool lightingOn;
	bool attenuationOn;
	ID3D11Buffer* constBuffer;
	std::vector<ID3D11SamplerState*> samplerState;
	std::vector<ID3D11ShaderResourceView*> shaderResourceView;
};
