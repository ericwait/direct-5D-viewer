#pragma once

#include "Renderer.h"

class Texture
{
public:
	virtual ID3D11SamplerState* getSampler() = 0;
	virtual ID3D11ShaderResourceView* getShaderResource() = 0;

	virtual ~Texture(){}

protected:
	Texture(){}

private:
	Texture(const Texture& other){}
};


class ConstTextureBase : public Texture
{
protected:
	ConstTextureBase(Renderer* rendererIn);

	virtual ID3D11SamplerState* getSampler() { return samplerState; }
	virtual ID3D11ShaderResourceView* getShaderResource() { return resourceView; }
	virtual ~ConstTextureBase();

private:
	ConstTextureBase(){}

protected:
	Renderer* renderer;

	ID3D11ShaderResourceView* resourceView;
	ID3D11SamplerState* samplerState;
};


class Const3DTexture : public ConstTextureBase
{
public:
	Const3DTexture(Renderer* rendererIn, Vec<size_t> dims, const unsigned char* texData);

private:
	Const3DTexture();
	
	Vec<size_t> dims;
};


class TextAtlasTexture : public ConstTextureBase
{
public:
	TextAtlasTexture(Renderer* rendererIn, HWND hwnd, const std::string& fontFace, int textHeight, const std::string& charList);

	//TESTING!
	virtual ~TextAtlasTexture(){delete[] atlas;}

private:
	bool computeAtlasSize();
	size_t createTextAtlas(HWND hwnd, const std::string& fontFace, int textHeight, const std::string& charList, uint8_t** atlasOut);

	//TESTING
	uint8_t* atlas;


	int numChars;
	// Total texture dimensions in pixels
	Vec<size_t> atlasDims;
	// Total number of character boxes in atlas
	Vec<size_t> atlasBoxes;
	// Box dimensions of a character in the atlas
	Vec<size_t> charBox;

	// Actual character size for layout
	Vec<size_t> charSize;
};
