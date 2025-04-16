#pragma once

#include "Renderer.h"
#include "stb_truetype.h"

#include <vector>

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


class TextRenderer;

class TextAtlasTexture : public ConstTextureBase
{
	friend class TextRenderer;
public:
	TextAtlasTexture(Renderer* rendererIn, const std::string& fontFace, int textHeightPx, int charStart, int charCount);

	bool getWhitespaceOffset(int c, float lineStart, Vec<float>& cursor);
	bool getCharQuad(int c, Vec<float>& cursor, Vec<float> quadPos[4], Vec<float> quadUVs[4]);

private:
	bool computeAtlasSize(int numChars, Vec<int> charBox);
	size_t bakeTextAtlas(const std::string& fontFace, int textHeightPx, int charStart, int charCount);

	// Total texture dimensions in pixels
	Vec<size_t> atlasDims;
	std::vector<uint8_t> atlas;

	int atlasStart;
	Vec<int> maxCharBox;
	Vec<float> wsOffset;
	std::vector<stbtt_bakedchar> charInfo;
};
