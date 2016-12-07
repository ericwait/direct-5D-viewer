#pragma once

#include "Renderer.h"

class Texture
{
public:
	Texture(Renderer* rendererIn, Vec<size_t> dims, const unsigned char* texData);
	~Texture();



	ID3D11SamplerState* getSampler(){return sampler;}
	ID3D11ShaderResourceView* getResource(){return resource;}

protected:
	Texture(){};

	Renderer* renderer;

	ID3D11SamplerState* sampler;
	ID3D11ShaderResourceView* resource;
};
