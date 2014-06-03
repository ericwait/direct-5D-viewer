#include "Material.h"

SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn) : Material(rendererIn)
{
	colorBuffer.color = DirectX::XMFLOAT4(1,1,1,1);
	colorBuffer.colorModifier = DirectX::XMFLOAT4(1,1,1,1);

	renderer->createConstantBuffer(sizeof(ColorBuffer), &constBuffer);

	setShader("DefaultMeshShaders.fx","DefaultMeshPixelShader");
}

SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn, Vec<float> colorIn) : Material(rendererIn)
{
	colorBuffer.color = DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z,1);
	colorBuffer.colorModifier = DirectX::XMFLOAT4(1,1,1,1);

	renderer->createConstantBuffer(sizeof(ColorBuffer), &constBuffer);

	setShader("DefaultMeshShaders.fx","DefaultMeshPixelShader");
}

SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn, Vec<float> colorIn, float alpha) : Material(rendererIn)
{
	colorBuffer.color = DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z,alpha);
	colorBuffer.colorModifier = DirectX::XMFLOAT4(1,1,1,1);

	renderer->createConstantBuffer(sizeof(ColorBuffer), &constBuffer);

	setShader("DefaultMeshShaders.fx","DefaultMeshPixelShader");
}

SingleColoredMaterial::~SingleColoredMaterial()
{
	SAFE_RELEASE(constBuffer);

	Material::~Material();
}

void SingleColoredMaterial::setColor(Vec<float> colorIn, float alpha)
{
	colorBuffer.color = DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z,alpha);
}

void SingleColoredMaterial::setColorModifier(Vec<float> colorMod, float alphaMod)
{
	colorBuffer.colorModifier = DirectX::XMFLOAT4(colorMod.x,colorMod.y,colorMod.z,alphaMod);
}

void SingleColoredMaterial::updateParams()
{
	renderer->updateShaderParams(&colorBuffer,constBuffer);
}