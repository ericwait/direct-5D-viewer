#include "Texture.h"

#include <stdint.h>
#include <fstream>
#include <iterator>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

ConstTextureBase::ConstTextureBase(Renderer* rendererIn)
	: renderer(rendererIn), resourceView(NULL), samplerState(NULL)
{}

ConstTextureBase::~ConstTextureBase()
{
	SAFE_RELEASE(resourceView);
	SAFE_RELEASE(samplerState);
}



Const3DTexture::Const3DTexture(Renderer* rendererIn, Vec<size_t> dims, const unsigned char* texData)
	: ConstTextureBase(rendererIn), dims(dims)
{
	UINT iMipCount = 1;
	UINT BitSize = 0;

	D3D11_TEXTURE3D_DESC desc;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.Width = (unsigned int)dims.x;
	desc.Height = (unsigned int)dims.y;
	desc.Depth = (unsigned int)dims.z;
	desc.MipLevels = iMipCount;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.SysMemPitch = (unsigned int)dims.x;
	initData.SysMemSlicePitch = unsigned int(dims.x*dims.y);
	initData.pSysMem = (void*)texData;

	ID3D11Resource* texture3D = renderer->createTexture3D(&desc, &initData);

	resourceView = renderer->createShaderResourceView(texture3D);
	samplerState = rendererIn->createSamplerState(InterpTypes::Linear);

	SAFE_RELEASE(texture3D);
}



TextAtlasTexture::TextAtlasTexture(Renderer* rendererIn, const std::string& fontFace, int textHeight, int char_start, int char_count)
	: ConstTextureBase(rendererIn)
{
	// Use GDI to render the character list to a memory buffer.
	//uint8_t* atlas = NULL;
	size_t atlasSize = bakeTextAtlas(fontFace, textHeight, char_start, char_count);
	if ( atlasSize == 0 )
		std::runtime_error("Unable to create atlas for font!");

	// Create 2D texture from memory buffer
	UINT iMipCount = 1;
	UINT BitSize = 0;

	D3D11_TEXTURE2D_DESC desc;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.Width = atlasDims.x;
	desc.Height = atlasDims.y;
	desc.MipLevels = iMipCount;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.SysMemPitch = atlasDims.x;
	initData.SysMemSlicePitch = atlasDims.y;
	initData.pSysMem = (void*)atlas.data();

	ID3D11Resource* texture2D = renderer->createTexture2D(&desc, &initData);

	resourceView = renderer->createShaderResourceView(texture2D);
	samplerState = rendererIn->createSamplerState(InterpTypes::Nearest);

	SAFE_RELEASE(texture2D);
};

bool TextAtlasTexture::getWhitespaceOffset(int c, float lineStart, Vec<float>& cursor)
{
	float pad = 0.0f;
	if (c == '\n')
	{
		cursor.x = lineStart;
		cursor.y += pad + wsOffset.y;
		return true;
	}

	if (c == ' ')
	{
		cursor += Vec<float>(pad + wsOffset.x, 0.0f, 0.0f);
		return true;
	}

	return false;
}

bool TextAtlasTexture::getCharQuad(int c, Vec<float>& cursor, Vec<float> quadPos[4], Vec<float> quadUVs[4])
{
	int atlasIdx = c - atlasStart;

	// TODO: Changed this from old method, don't advance for unrecognized char
	if (atlasIdx < 0 || atlasIdx >= charInfo.size())
		return false;

	stbtt_aligned_quad q;
	stbtt_GetBakedQuad(charInfo.data(), atlasDims.x, atlasDims.y, atlasIdx, &cursor.x, &cursor.y, &q, 1);
	
	quadPos[0].x = q.x0;
	quadPos[0].y = q.y0;
	quadUVs[0].x = q.s0;
	quadUVs[0].y = q.t0;

	quadPos[1].x = q.x0;
	quadPos[1].y = q.y1;
	quadUVs[1].x = q.s0;
	quadUVs[1].y = q.t1;

	quadPos[2].x = q.x1;
	quadPos[2].y = q.y1;
	quadUVs[2].x = q.s1;
	quadUVs[2].y = q.t1;

	quadPos[3].x = q.x1;
	quadPos[3].y = q.y0;
	quadUVs[3].x = q.s1;
	quadUVs[3].y = q.t0;

	return true;
}

#define CEIL_DIV(n,d) (((n)+(d)-1)/(d))
#define ROUND_DIV(n,d) (((n)+(n)+(d))/((d)+(d)))

bool TextAtlasTexture::computeAtlasSize(int numChars, Vec<int> charBox)
{
	const int minDim = 32;
	const int maxDim = 2048;

	// Figure out a square-ish texture size for the atlas
	int width = 0;
	for ( int w = minDim; w <= maxDim; w = w << 1 )
	{
		int boxes = (w / charBox.x) * (w / charBox.y);
		if ( boxes < numChars )
			continue;

		width = w;
		break;
	}

	if ( width == 0 )
		return false;

	int minRows = CEIL_DIV(numChars, (width / charBox.x));
	int minHeight = minRows * charBox.y;

	// Find the smallest power of 2 height that works
	int height = 0;
	for ( int h = minDim; h <= maxDim; h = h << 1 )
	{
		if ( h < minHeight )
			continue;

		height = h;
		break;
	}

	atlasDims = Vec<size_t>(width,height,1);

	return true;
}

#pragma optimize("",off)
size_t TextAtlasTexture::bakeTextAtlas(const std::string& fontFace, int textHeightPx, int charStart, int charCount)
{
	// NOTE: We are assuming we are using only ascii characters 
	atlasStart = charStart;

	std::ifstream fontStream(fontFace, std::ios::binary);
	std::vector<uint8_t> fontData{std::istreambuf_iterator<char>(fontStream), std::istreambuf_iterator<char>()};

	stbtt_fontinfo fontInfo;
	stbtt_InitFont(&fontInfo, fontData.data(), 0);

	// Compute scale for codepoint metrics
	float scale = stbtt_ScaleForPixelHeight(&fontInfo, textHeightPx);

	// TODO: Figure out actual widest char or do more generalized font bake/packing
	// Assume 'W' is widest char
	int maxAdvance;
	stbtt_GetCodepointHMetrics(&fontInfo, 'W', &maxAdvance, nullptr);

	maxCharBox.x = static_cast<int>(maxAdvance*scale + 1.0f);
	maxCharBox.y = textHeightPx;
	if (!computeAtlasSize(charCount, maxCharBox))
	{
		return 0;
	}

	charInfo.resize(charCount);
	atlas.resize(atlasDims.product());

	// Compute the x/y offset for white-space (i.e. ' ' / '\n')
	int advance;
	stbtt_GetCodepointHMetrics(&fontInfo, ' ', &advance, nullptr);

	int ascent;
	int descent;
	int lineGap;
	stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

	wsOffset.x = advance * scale;
	wsOffset.y = (ascent - descent + lineGap) * scale;

	stbtt_BakeFontBitmap(fontData.data(), 0, textHeightPx,
						atlas.data(), atlasDims.x, atlasDims.y,
						charStart, charCount, charInfo.data());

	return charInfo.size();
}
