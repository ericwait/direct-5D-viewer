#include "Texture.h"

Texture::Texture(Renderer* rendererIn, Vec<size_t> dims, const unsigned char* texData)
{
	renderer = rendererIn;

	sampler = renderer->getSamplerState();
	resource = renderer->createTextureResourceView(dims, texData);
}

Texture::~Texture()
{
	SAFE_RELEASE(resource);
}
