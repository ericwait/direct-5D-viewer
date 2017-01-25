#pragma once

#include "Renderer.h"

class Texture
{
public:
	Texture(Renderer* rendererIn);

	ID3D11SamplerState* getSampler(){return samplerState;}
	virtual ID3D11ShaderResourceView* getShaderResource() = 0;

	virtual ~Texture();

private:
	Texture(){};
	Texture(const Texture& other){}

	ID3D11SamplerState* samplerState;
};


class ConstTexture : public Texture
{
public:
	ConstTexture(Renderer* rendererIn, Vec<size_t> dims, const unsigned char* texData);

	virtual ID3D11ShaderResourceView* getShaderResource(){return resourceView;}
	virtual ~ConstTexture();

private:
	ConstTexture();

	Renderer* renderer;
	Vec<size_t> dims;

	ID3D11ShaderResourceView* resourceView;
};

