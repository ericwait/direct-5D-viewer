#include "Material.h"

Material::Material(Renderer* rendererIn)
{
	renderer = rendererIn;
	shaderIdx = -1;
	wireframe = false;
	cullBackFace = true;
	testDepth = true;
}

Material::~Material()
{
	renderer = NULL;
	shaderIdx = -1;
	wireframe = false;
}

void Material::setWireframe(bool wireframe)
{
	this->wireframe = wireframe;
}

void Material::setShader(const std::string& shaderFilename, const std::string& shaderFunction, const std::string& shaderParams)
{
	shaderIdx = renderer->getPixelShader(shaderFilename,shaderFunction,shaderParams);
}
