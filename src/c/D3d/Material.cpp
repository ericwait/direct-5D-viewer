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

void createStaticVolumeShaderText(std::string strChans, Renderer* renderer);


Material::Material(Renderer* rendererIn)
{
	renderer = rendererIn;

	shaderIdx = -1;
	wireframe = false;
	cullBackFace = true;
	testDepth = true;

	params = NULL;
}

Material::Material(Renderer* rendererIn, std::weak_ptr<MaterialParameters> sharedParams)
{
	renderer = rendererIn;

	shaderIdx = -1;
	wireframe = false;
	cullBackFace = true;
	testDepth = true;

	params = sharedParams.lock();
}

Material::~Material()
{
	renderer = NULL;
	shaderIdx = -1;
	wireframe = false;
}

void Material::attachTexture(int slot, std::shared_ptr<Texture>& texture)
{
	// TODO: Should we throw this or just resize?
	if ( slot >= textures.size() )
	{
		char errBuf[256];
		sprintf(errBuf, "Cannot attach to slot %d, only %zu slots are declared for this material", slot, textures.size());

		throw std::runtime_error(errBuf);
	}

	textures[slot] = texture;
}

void Material::setWireframe(bool wireframe)
{
	this->wireframe = wireframe;
}

void Material::setShader(const std::string& shaderFilename, const std::string& shaderFunction, const std::string& shaderParams)
{
	shaderIdx = renderer->getPixelShader(shaderFilename,shaderFunction,shaderParams);
}

void Material::bindConstants()
{
	if ( params )
		params->setShaderResources();
}

void Material::bindTextures()
{
	std::vector<ID3D11SamplerState*> samplers;
	std::vector<ID3D11ShaderResourceView*> resources;

	for ( int i=0; i < textures.size(); ++i )
	{
		samplers.push_back(textures[i]->getSampler());
		resources.push_back(textures[i]->getResource());
	}

	renderer->setPixelShaderResourceViews(0, (int)resources.size(), resources.data());
	renderer->setPixelShaderTextureSamplers(0, (int)samplers.size(), samplers.data());
}



SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn)
	: Material(rendererIn)
{
	params = std::make_shared<SingleColorParams>(rendererIn, Vec<float>(1.0f,1.0f,1.0f), 1.0f);

	std::string root = renderer->getDllDir();
	setShader(root + PIXEL_SHADER_FILENAMES[Renderer::PixelShaders::DefaultPS],
		PIXEL_SHADER_FUNCNAMES[Renderer::PixelShaders::DefaultPS],"");
}

SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn, Vec<float> colorIn, float alpha) : Material(rendererIn)
{
	params = std::make_shared<SingleColorParams>(rendererIn, colorIn, alpha);

	std::string root = renderer->getDllDir();
	setShader(root + PIXEL_SHADER_FILENAMES[Renderer::PixelShaders::DefaultPS],
		PIXEL_SHADER_FUNCNAMES[Renderer::PixelShaders::DefaultPS],"");
}

void SingleColoredMaterial::setColor(Vec<float> colorIn, float alpha)
{
	static_cast<SingleColorParams*>(params.get())->setColor(colorIn, alpha);
}

void SingleColoredMaterial::setColorModifier(Vec<float> colorMod, float alphaMod)
{
	static_cast<SingleColorParams*>(params.get())->setColorModifier(colorMod, alphaMod);
}

void SingleColoredMaterial::setLightOn(bool on)
{
	static_cast<SingleColorParams*>(params.get())->setLightOn(on);
}



StaticVolumeTextureMaterial::StaticVolumeTextureMaterial(Renderer* rendererIn, int numChannelsIn, std::weak_ptr<StaticVolumeParams> paramsIn)
	: Material(rendererIn, paramsIn)
{
	cullBackFace = false;
	testDepth = false;
	wireframe = false;

	numChannels = numChannelsIn;
	textures.resize(numChannels);

	char cBuffer[3];
	sprintf_s(cBuffer, "%d", numChannels);
	std::string strChans = cBuffer;
	createStaticVolumeShaderText(strChans,renderer);

	std::string root = renderer->getDllDir();
	setShader(root + PIXEL_SHADER_FILENAMES[Renderer::PixelShaders::StaticVolume],
		PIXEL_SHADER_FUNCNAMES[Renderer::PixelShaders::StaticVolume],strChans);
}

void StaticVolumeTextureMaterial::setTransferFunction(int channel, Vec<float> transferFunction)
{
	static_cast<StaticVolumeParams*>(params.get())->setTransferFunction(channel, transferFunction);
}

void StaticVolumeTextureMaterial::setRange(int channel, Vec<float> ranges)
{
	static_cast<StaticVolumeParams*>(params.get())->setRange(channel, ranges);
}

void StaticVolumeTextureMaterial::setColor(int channel, Vec<float> color, float alphaMod)
{
	static_cast<StaticVolumeParams*>(params.get())->setColor(channel, color, alphaMod);
}

void StaticVolumeTextureMaterial::setGradientSampleDir(Vec<float> xDir, Vec<float> yDir, Vec<float> zDir)
{
	static_cast<StaticVolumeParams*>(params.get())->setGradientSampleDir(xDir, yDir, zDir);
}

void StaticVolumeTextureMaterial::setLightOn(bool on)
{
	static_cast<StaticVolumeParams*>(params.get())->setLightOn(on);
}

void StaticVolumeTextureMaterial::setAttenuationOn(bool on)
{
	static_cast<StaticVolumeParams*>(params.get())->setAttenuationOn(on);
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
	shaderText += "	float4 flags;\n";
	shaderText += "	float4 transferFunctions[" + strChans + "];\n";
	shaderText += "	float4 ranges[" + strChans + "];\n";
	shaderText += "	float4 channelColor[" + strChans + "];\n";
	shaderText += "	float4 gradientSampleDirection[3];\n";
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
	shaderText += "\t\tif(flags.x>0)\n";
	shaderText += "\t\t{\n";
	shaderText += "\t\t\tgrad.x = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[0]) - \n";
	shaderText += "\t\t\t\tg_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[0]);\n";
	shaderText += "\t\t\tgrad.y = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[1]) - \n";
	shaderText += "\t\t\t\tg_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[1]);\n";
	shaderText += "\t\t\tgrad.z = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[2]) - \n";
	shaderText += "\t\t\t\tg_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[2]);\n";
	shaderText += "\t\t\tgrad = normalize(grad);\n";
    shaderText += "\t\t\tfloat ambientLight = 0.45f;\n";
	shaderText += "\t\t\tlightMod = saturate(dot(grad,mainLightDir)*2.0*(1-ambientLight) + ambientLight);\n";
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
	shaderText += "\tif(flags.y>0)\n";
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
