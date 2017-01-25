#include "Texture.h"

Texture::Texture(Renderer* rendererIn)
{
	samplerState = rendererIn->createSamplerState();
}

Texture::~Texture()
{
	SAFE_RELEASE(samplerState);
}



ConstTexture::ConstTexture(Renderer* rendererIn, Vec<size_t> dims, const unsigned char* texData)
	: Texture(rendererIn), renderer(rendererIn), dims(dims)
{
	UINT iMipCount = 1;
	UINT BitSize = 0;

	D3D11_TEXTURE3D_DESC desc;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.Width = (unsigned int)dims.x;
	desc.Height = (unsigned int)dims.y;
	desc.Depth = (unsigned int)dims.z;
	desc.MipLevels = iMipCount;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.SysMemPitch = (unsigned int)dims.x;
	initData.SysMemSlicePitch = unsigned int(dims.x*dims.y);
	initData.pSysMem = (void*)texData;

	ID3D11Resource* texture3D = renderer->createTexture3D(&desc, &initData);

	resourceView = renderer->createShaderResourceView(texture3D);

	SAFE_RELEASE(texture3D);
}

ConstTexture::~ConstTexture()
{
	SAFE_RELEASE(resourceView);
}

