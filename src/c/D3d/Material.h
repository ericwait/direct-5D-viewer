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
#include "Global/Vec.h"
#include "Renderer.h"

#include "MaterialParams.h"
#include "Texture.h"

#include <DirectXMath.h>
#include <memory>

class Material
{
public:
	friend class Renderer;
	virtual ~Material();


	void attachTexture(int slot, std::shared_ptr<Texture> texture);


	// Return generalized material parameter structure
	MaterialParameters* getParams(){return params.get();}

	// Return requested material parameters type
	template<class T> T* typedParams(){return dynamic_cast<T*>(params.get());}


	void setWireframe(bool wireframe);
	virtual DirectX::XMFLOAT4 getColor(){return DirectX::XMFLOAT4(0.0f,0.0f,0.0f,0.0f);}

	// Overloaded to potentially pass transform related variables to the pixel shader
	virtual void updateTransformParams(DirectX::XMMATRIX localToWorld, DirectX::XMMATRIX view, DirectX::XMMATRIX projection){}

protected:
	Material(){}
	Material(Renderer* rendererIn);
	Material(Renderer* rendererIn, std::shared_ptr<MaterialParameters> sharedParams);

	void setShader(const std::string& shaderFilename, const std::string& shaderFunction, const std::map<std::string,std::string>& variables = std::map<std::string,std::string>());
	void bindConstants();
	void bindTextures();

	void setMaterialProps(bool wireframe, bool cullBackface, bool depthTest);

	Renderer* renderer;

	// We use a ref count so that parameters can be shared across materials
	std::shared_ptr<MaterialParameters> params;

	// Shared texture references list
	std::vector<std::shared_ptr<Texture>> textures;

private:
	void updateRasterState();

	bool wireframe;
	bool cullBackFace;
	bool testDepth;

	int shaderIdx;

	ID3D11RasterizerState* rasterState;
	ID3D11DepthStencilState* depthStencilState;
};


class SingleColoredMaterial : public Material
{
public:
	SingleColoredMaterial(Renderer* renderer);
	SingleColoredMaterial(Renderer* renderer, Vec<float> colorIn, float alpha = 1.0f);

	void setColor(Vec<float> colorIn, float alpha);
	void setColorModifier(Vec<float> colorMod, float alphaMod);

	void setLightOn(bool on);

private:
	SingleColoredMaterial(){}
};


class StaticVolumeTextureMaterial : public Material
{
public:
	StaticVolumeTextureMaterial(Renderer* rendererIn, int numChannelsIn, Vec<size_t> dims, std::shared_ptr<StaticVolumeParams> paramsIn);

	// Overloaded to potentially pass transform related variables to the pixel shader
	virtual void updateTransformParams(DirectX::XMMATRIX localToWorld, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);

private:
	StaticVolumeTextureMaterial(){};

	int numChannels;
	Vec<size_t> dims;
};
