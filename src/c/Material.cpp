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

const Vec<float> StaticVolumeTextureMaterial::colors[6] = 
{
	Vec<float>(1.0f, 0.0f, 0.0f),
	Vec<float>(0.0f, 1.0f, 0.0f),
	Vec<float>(0.0f, 0.0f, 1.0f),
	Vec<float>(1.0f, 1.0f, 0.0f),
	Vec<float>(0.0f, 1.0f, 1.0f),
	Vec<float>(1.0f, 0.0f, 1.0f)
};


void createStaticVolumeShaderText(std::string strChans, Renderer* renderer);


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



SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn) : Material(rendererIn)
{
	colorBuffer.color = DirectX::XMFLOAT4(1,1,1,1);
	colorBuffer.colorModifier = DirectX::XMFLOAT4(1,1,1,1);

	renderer->createConstantBuffer(sizeof(ColorBuffer), &constBuffer);

	std::string root = renderer->getDllDir();
	setShader(root + PIXEL_SHADER_FILENAMES[Renderer::PixelShaders::DefaultPS],
		PIXEL_SHADER_FUNCNAMES[Renderer::PixelShaders::DefaultPS],"");
}

SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn, Vec<float> colorIn) : Material(rendererIn)
{
	colorBuffer.color = DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z,1);
	colorBuffer.colorModifier = DirectX::XMFLOAT4(1,1,1,1);

	renderer->createConstantBuffer(sizeof(ColorBuffer), &constBuffer);

	std::string root = renderer->getDllDir();
	setShader(root + PIXEL_SHADER_FILENAMES[Renderer::PixelShaders::DefaultPS],
		PIXEL_SHADER_FUNCNAMES[Renderer::PixelShaders::DefaultPS],"");
}

SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn, Vec<float> colorIn, float alpha) : Material(rendererIn)
{
	colorBuffer.color = DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z,alpha);
	colorBuffer.colorModifier = DirectX::XMFLOAT4(1,1,1,1);

	renderer->createConstantBuffer(sizeof(ColorBuffer), &constBuffer);

	std::string root = renderer->getDllDir();
	setShader(root + PIXEL_SHADER_FILENAMES[Renderer::PixelShaders::DefaultPS],
		PIXEL_SHADER_FUNCNAMES[Renderer::PixelShaders::DefaultPS],"");
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

void SingleColoredMaterial::setShaderResources()
{
	renderer->setPixelShaderConsts(constBuffer);
}

void SingleColoredMaterial::updateParams()
{
	renderer->updateShaderParams(&colorBuffer,constBuffer);
}



StaticVolumeTextureMaterial::StaticVolumeTextureMaterial(Renderer* rendererIn, Vec<size_t> dimsIn, int numChannelsIn, unsigned char* image,
	unsigned char** shaderConstMemoryIn/*=NULL*/) : Material(rendererIn)
{
	numChannels = numChannelsIn;
	dims = dimsIn;
	shaderConstMemory = *shaderConstMemoryIn;

	cullBackFace = false;
	testDepth = false;
	wireframe = false;
	lightingOn = false;
	attenuationOn = false;

	char cBuffer[3];
	sprintf_s(cBuffer, "%d", numChannels);
	std::string strChans = cBuffer;
	createStaticVolumeShaderText(strChans,renderer);

	volPixShaderConsts.channelColors.resize(numChannels);
	volPixShaderConsts.ranges.resize(numChannels);
	volPixShaderConsts.transferFunctions.resize(numChannels);

	if (shaderConstMemory == NULL)
	{
		shaderConstMemory = new unsigned char[volPixShaderConsts.sizeOf()];
		*shaderConstMemoryIn = shaderConstMemory;

		for (int i = 0; i < numChannels; ++i)
		{
			setTransferFunction(i, Vec<float>(0.0f, 1.0f, 0.0f));
			setRange(i, Vec<float>(0.0f, 1.0f, 0.0f));
			setColor(i, colors[i % 6], 1.0f);
			setAttenuationOn(false);
			setLightOn(false);
		}
	}
	renderer->createConstantBuffer(volPixShaderConsts.sizeOf(),&constBuffer);

	std::string root = renderer->getDllDir();
	setShader(root + PIXEL_SHADER_FILENAMES[Renderer::PixelShaders::StaticVolume],
		PIXEL_SHADER_FUNCNAMES[Renderer::PixelShaders::StaticVolume],strChans);

	samplerState.resize(numChannels);
	shaderResourceView.resize(numChannels);

	for (int i=0; i<numChannels; ++i)
	{
		samplerState[i] = renderer->getSamplerState();
		shaderResourceView[i] = renderer->createTextureResourceView(dims, image + dims.product()*i);
	}
}

StaticVolumeTextureMaterial::~StaticVolumeTextureMaterial()
{
	for(int i=0; i<shaderResourceView.size(); ++i)
	{
		SAFE_RELEASE(shaderResourceView[i]);
	}

	SAFE_RELEASE(constBuffer);

	//delete[] shaderConstMemory;  //TODO make a better structure that cleans this up

	Material::~Material();
}

void StaticVolumeTextureMaterial::setTransferFunction(int channel, Vec<float> transferFunction)
{
	//channel number of float4s before it
	size_t memStart = channel*sizeof(float)*4;

	memcpy((void*)(shaderConstMemory+memStart),&(transferFunction),sizeof(float)*3);
}

void StaticVolumeTextureMaterial::setRange(int channel, Vec<float> ranges)
{
	//channel number of float4 before it along with numChannel float4s behind that
	size_t memStart = channel*sizeof(float)*4 + sizeof(float)*4*numChannels;

	memcpy((void*)(shaderConstMemory+memStart),&(ranges),sizeof(float)*3);
}

void StaticVolumeTextureMaterial::setColor(int channel, Vec<float> color, float alphaMod)
{
	//channel number of float4 before it along with 2 times numchannel float4s behind that
	size_t memStart = channel*sizeof(float)*4 + 8*sizeof(float)*numChannels;

	memcpy((void*)(shaderConstMemory+memStart),&(color.x),sizeof(float)*3);
	memcpy((void*)(shaderConstMemory+memStart+sizeof(float)*3),&(alphaMod),sizeof(float));
}

void StaticVolumeTextureMaterial::setLightOn(bool on)
{
	lightingOn = on;

	float isOn = (on==true) ? (1.0f) : (0.0f);
	//3 float4s along with 3 times numchannel float4s behind
	size_t memStart = 12*sizeof(float)+12*sizeof(float)*numChannels;

	memcpy((void*)(shaderConstMemory+memStart),&isOn,sizeof(float));
}

void StaticVolumeTextureMaterial::setAttenuationOn(bool on)
{
	attenuationOn = on;

	float isOn = (on == true) ? (1.0f) : (0.0f);
	//2nd float plus 3 float4s along with 3 times numchannel float4s behind
	size_t memStart = sizeof(float) + 12 * sizeof(float) + 12 * sizeof(float)*numChannels;

	memcpy((void*)(shaderConstMemory + memStart), &isOn, sizeof(float));
}


void StaticVolumeTextureMaterial::setGradientSampleDir(Vec<float> xDir, Vec<float> yDir, Vec<float> zDir)
{
	//3 times numchannel float4s behind
	size_t memStart = 12*sizeof(float)*numChannels;

	memcpy((void*)(shaderConstMemory+memStart),&xDir,sizeof(float)*3);
	memcpy((void*)(shaderConstMemory+memStart+sizeof(float)*4),&yDir,sizeof(float)*3);
	memcpy((void*)(shaderConstMemory+memStart+sizeof(float)*8),&zDir,sizeof(float)*3);
}

void StaticVolumeTextureMaterial::updateParams()
{
	renderer->updateShaderParams(shaderConstMemory,constBuffer);
}

void StaticVolumeTextureMaterial::setShaderResources()
{
	renderer->setPixelShaderConsts(constBuffer);
	renderer->setPixelShaderResourceViews(0,(int)shaderResourceView.size(),shaderResourceView.data());
	renderer->setPixelShaderTextureSamplers(0,(int)samplerState.size(),samplerState.data());
}

void createStaticVolumeShaderText(std::string strChans,Renderer* renderer)
{
	std::string shaderText = "";

	shaderText += "///////////////////////////////////////////////////////////////////////////////\n";
	shaderText += "// Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter\n";
	shaderText += "// This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,\n";
	shaderText += "// tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section\n";
	shaderText += "// for details. LEVER 3-D is free software: you can redistribute it and/or modify\n";
	shaderText += "// it under the terms of the GNU General Public License as published by the Free\n";
	shaderText += "// Software Foundation, either version 3 of the License, or (at your option) any\n";
	shaderText += "// later version.\n";
	shaderText += "// LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY\n";
	shaderText += "// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR\n";
	shaderText += "// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n";
	shaderText += "// You should have received a copy of the GNU General Public License along with\n";
	shaderText += "// LEVer in file \"gnu gpl v3.txt\".  If not, see  <http://www.gnu.org/licenses/>.\n";
	shaderText += "///////////////////////////////////////////////////////////////////////////////\n";
	shaderText += "\n";
	shaderText += "cbuffer ConstantPSBuffer : register( b1 )\n";
	shaderText += "{\n";
	shaderText += "	float4 transferFunctions[" + strChans + "];\n";
	shaderText += "	float4 ranges[" + strChans + "];\n";
	shaderText += "	float4 channelColor[" + strChans + "];\n";
	shaderText += "	float4 gradientSampleDirection[3];\n";
	shaderText += "	float4 lightOn;\n";
	shaderText += "};\n";
	shaderText += "\n";
	shaderText += "struct PixelOutputType\n";
	shaderText += "{\n";
	shaderText += "\tfloat4 color : SV_TARGET;\n";
	shaderText += "\tfloat depth : SV_DEPTH;\n";
	shaderText += "};\n";
	shaderText += "\n";
	shaderText += "Texture3D    g_txDiffuse[" + strChans + "] : register( t0 );\n";
	shaderText += "SamplerState g_samLinear[" + strChans + "] : register( s0 );\n";
	shaderText += "\n";
	shaderText += "struct VS_OUTPUT\n";
	shaderText += "{\n";
	shaderText += "\tfloat4 Pos : SV_POSITION;\n";
	shaderText += "\tfloat3 TextureUV : TEXCOORD0;\n";
	shaderText += "\tfloat3 Normal : NORMAL;\n";
	shaderText += "\tfloat3 Dpth : TEXCOORD1;\n";
	shaderText += "};\n";
	shaderText += "\n";
	shaderText += "PixelOutputType MultiChanVolumePixelShader( VS_OUTPUT input )\n";
	shaderText += "{\n";
	shaderText += "\tPixelOutputType output = (PixelOutputType)0;\n";
	shaderText += "\tfloat alpha = 0.0f;\n";
	shaderText += "\n";
	shaderText += "\tfloat4 mainLightDir = float4(-0.5774,-0.5774,0.5774,0);\n";
	shaderText += "\tfloat3 unlitComposite = float3(0.0f,0.0f,0.0f);\n";
	shaderText += "\tfloat3 alphaComposite = float3(0.0f,0.0f,0.0f);\n";
	shaderText += "\n";
	shaderText += "\tint numAlpha = 0;\n";
	shaderText += "\tfloat3 grad;\n";
	shaderText += "\n";
	shaderText += "float maxIntensity = 0;\n";
	shaderText += "\n";
	shaderText += "\t[unroll(" + strChans + ")] for (int i=0; i<" + strChans + "; ++i)\n";
	shaderText += "\t{\n";
	shaderText += "\t\tfloat intensity = g_txDiffuse[i].Sample( g_samLinear[i], input.TextureUV );\n";
	shaderText += "\t\tintensity = clamp(intensity,ranges[i][0],ranges[i][1]);\n";
	shaderText += "\t\tintensity = transferFunctions[i][0]*intensity*intensity+transferFunctions[i][1]*intensity+transferFunctions[i][2];\n";
	shaderText += "\t\tif (intensity < 0.2f || channelColor[i].a < 0.01f)\n";
	shaderText += "\t\t{\n";
	shaderText += "\t\t\tintensity = 0;\n";
	shaderText += "\t\t}\n";
	//shaderText += "\t\tmaxIntensity = max(intensity*channelColor[i].a,maxIntensity);\n";
	shaderText += "\t\tmaxIntensity = max(intensity,maxIntensity);\n";
	shaderText += "\n";
	shaderText += "\t\tfloat lightMod = 1.0f;\n";
	shaderText += "\t\tif(lightOn.x>0)\n";
	shaderText += "\t\t{\n";
	shaderText += "\t\t\tgrad.x = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[0]) - \n";
	shaderText += "\t\t\t\tg_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[0]);\n";
	shaderText += "\t\t\tgrad.y = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[1]) - \n";
	shaderText += "\t\t\t\tg_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[1]);\n";
	shaderText += "\t\t\tgrad.z = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[2]) - \n";
	shaderText += "\t\t\t\tg_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[2]);\n";
	shaderText += "\t\t\tgrad = normalize(grad);\n";
	shaderText += "\t\t\tlightMod = saturate(dot(grad,mainLightDir)*2.0)*0.65 + 0.35;\n";
	shaderText += "\t\t}\n";
	shaderText += "\n";
	shaderText += "\t\toutput.color.rgb += (lightMod*intensity*channelColor[i].rgb);\n";
	shaderText += "\t\tunlitComposite += intensity*channelColor[i].rgb;\n";
	shaderText += "\t\talphaComposite += intensity*channelColor[i].rgb*channelColor[i].a;\n";
	shaderText += "\t}\n";
	shaderText += "\n";
	shaderText += "\tfloat maxComponent = max(max(unlitComposite.r,unlitComposite.g),unlitComposite.b);\n";
	shaderText += "\tfloat maxAplha = max(max(alphaComposite.r,alphaComposite.g),alphaComposite.b);\n";
	shaderText += "\tif (maxComponent!=0)\n";
	shaderText += "\t{\n";
	shaderText += "\t\toutput.color.rgb /= maxComponent;\n";
	shaderText += "\t\toutput.color.rgb *= maxIntensity;\n";
	shaderText += "\t}\n";
	shaderText += "\tif (maxAplha!=0)\n";
	shaderText += "\t{\n";
	shaderText += "\t\toutput.color.a = maxAplha;\n";
	shaderText += "\t}else{\n";
	shaderText += "\t\toutput.color.a = 0;\n";
	shaderText += "\t}\n";
	shaderText += "\n";
	shaderText += "\n";
	shaderText += "\tif(lightOn.y>0)\n";
	shaderText += "\t{\n";
	shaderText += "\t\tfloat distMult = (input.Dpth.z<=0) ? (1) : (1-input.Dpth.z);\n";
	shaderText += "\t\toutput.color.r *= distMult;\n";
	shaderText += "\t\toutput.color.g *= distMult;\n";
	shaderText += "\t\toutput.color.b *= distMult;\n";
	shaderText += "\t\toutput.color.a *= distMult;\n";
	shaderText += "\t}\n";
	shaderText += "\n";
	shaderText += "\toutput.depth = 1.0f;\n";
	shaderText += "\n";
	shaderText += "\treturn output;\n";
	shaderText += "}\n";

	std::string root = renderer->getDllDir();

	FILE* file;
	fopen_s(&file,(root + PIXEL_SHADER_FILENAMES[Renderer::PixelShaders::StaticVolume]).c_str(),"w");

	fprintf(file,shaderText.c_str());

	fclose(file);
}
