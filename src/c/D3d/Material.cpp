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

#include "Material.h"

Material::Material(Renderer* rendererIn)
	: renderer(rendererIn), shaderIdx(-1), rasterState(NULL)
{
	setMaterialProps(false, false, true);
}

Material::Material(Renderer* rendererIn, std::shared_ptr<MaterialParameters> sharedParams)
	: renderer(rendererIn), shaderIdx(-1), rasterState(NULL), params(sharedParams)
{
	setMaterialProps(false, false, true);
}

Material::~Material()
{
	renderer = NULL;
	shaderIdx = -1;
	wireframe = false;
}

void Material::attachTexture(int slot, std::shared_ptr<Texture> texture)
{
	// TODO: Should we throw this or just resize?
	if ( slot >= textures.size() )
	{
		char errBuf[256];
		sprintf(errBuf, "Cannot attach to slot %d, only %zu slots are declared for this material", slot, textures.size());

		throw std::runtime_error(errBuf);
	}

	textures[slot] = texture;
}

void Material::setWireframe(bool wireframe)
{
	this->wireframe = wireframe;

	updateRasterState();
}

void Material::setShader(const std::string& shaderFile, const std::string& shaderFunction, const std::map<std::string,std::string>& variables)
{
	shaderIdx = renderer->registerPixelShader(shaderFile, shaderFunction, variables);
}

void Material::bindConstants()
{
	if ( params )
		params->setShaderResources();
}

void Material::bindTextures()
{
	std::vector<ID3D11SamplerState*> samplers;
	std::vector<ID3D11ShaderResourceView*> resources;

	for ( int i=0; i < textures.size(); ++i )
	{
		samplers.push_back(textures[i]->getSampler());
		resources.push_back(textures[i]->getResource());
	}

	renderer->setPixelShaderResourceViews(0, (int)resources.size(), resources.data());
	renderer->setPixelShaderTextureSamplers(0, (int)samplers.size(), samplers.data());
}

void Material::setMaterialProps(bool wireframe, bool cullBackface, bool depthTest)
{
	this->wireframe = wireframe;
	this->cullBackFace = cullBackface;
	this->testDepth = depthTest;

	updateRasterState();
}

void Material::updateRasterState()
{
	rasterState	= renderer->getRasterizerState(wireframe, cullBackFace);
	depthStencilState = renderer->getDepthStencilState(testDepth);
}



SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn, Vec<float> colorIn, float alpha)
	: Material(rendererIn)
{
	params = std::make_shared<SingleColorParams>(rendererIn, colorIn, alpha);

	std::string root = renderer->getDllDir();
	setShader("DefaultMeshShaders","DefaultMeshPixelShader");
}

SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn)
	: SingleColoredMaterial(rendererIn, Vec<float>(1.0f,1.0f,1.0f), 1.0f)
{}

void SingleColoredMaterial::setColor(Vec<float> colorIn, float alpha)
{
	static_cast<SingleColorParams*>(params.get())->setColor(colorIn, alpha);
}

void SingleColoredMaterial::setColorModifier(Vec<float> colorMod, float alphaMod)
{
	static_cast<SingleColorParams*>(params.get())->setColorModifier(colorMod, alphaMod);
}

void SingleColoredMaterial::setLightOn(bool on)
{
	static_cast<SingleColorParams*>(params.get())->setLightOn(on);
}

DirectX::XMFLOAT4 SingleColoredMaterial::getColor()
{
	return params->getColor();
}



StaticVolumeTextureMaterial::StaticVolumeTextureMaterial(Renderer* rendererIn, int numChannelsIn, Vec<size_t> dims, std::shared_ptr<StaticVolumeParams> paramsIn)
	: Material(rendererIn, paramsIn), numChannels(numChannelsIn), dims(dims)
{
	setMaterialProps(false, false, false);
	
	textures.resize(numChannels);

	char cBuffer[3];
	sprintf_s(cBuffer, "%d", numChannels);

	std::map<std::string, std::string> vars;
	vars["NUM_CHAN"] = cBuffer;

	std::string root = renderer->getDllDir();
	setShader("StaticVolumePixelShader", "MultiChanVolumePixelShader", vars);
}

void StaticVolumeTextureMaterial::updateTransformParams(DirectX::XMMATRIX localToWorld, DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{
	DirectX::XMVECTOR det;
	DirectX::XMMATRIX invParentWorld = DirectX::XMMatrixInverse(&det,localToWorld);

	DirectX::XMMATRIX handedCorrection(0.0f,1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f);

	DirectX::XMFLOAT3 vec(1.0f,0.0f,0.0f);
	DirectX::XMVECTOR vecU = DirectX::XMLoadFloat3(&vec);
	DirectX::XMMATRIX trans = invParentWorld * handedCorrection * DirectX::XMMatrixScaling(1.0f/dims.x,1.0f/dims.y,1.0f/dims.z);

	Vec<float> xDir, yDir, zDir;
	DirectX::XMVECTOR vecO = DirectX::XMVector3TransformNormal(vecU,trans);
	xDir = Vec<float>(DirectX::XMVectorGetX(vecO),DirectX::XMVectorGetY(vecO),DirectX::XMVectorGetZ(vecO));
	vec = DirectX::XMFLOAT3(0.0f,1.0f,0.0f);
	vecU = DirectX::XMLoadFloat3(&vec);
	vecO = DirectX::XMVector3TransformNormal(vecU,trans);
	yDir = Vec<float>(DirectX::XMVectorGetX(vecO),DirectX::XMVectorGetY(vecO),DirectX::XMVectorGetZ(vecO));
	vec = DirectX::XMFLOAT3(0.0f,0.0f,1.0f);
	vecU = DirectX::XMLoadFloat3(&vec);
	vecO = DirectX::XMVector3TransformNormal(vecU,trans);
	zDir = Vec<float>(DirectX::XMVectorGetX(vecO),DirectX::XMVectorGetY(vecO),DirectX::XMVectorGetZ(vecO));

	typedParams<StaticVolumeParams>()->setGradientSampleDir(xDir,yDir,zDir);
}
