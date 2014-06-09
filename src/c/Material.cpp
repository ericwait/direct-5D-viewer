#include "Material.h"

const Vec<float> colors[6] = 
{
	Vec<float>(1.0f, 0.0f, 0.0f),
	Vec<float>(0.0f, 1.0f, 0.0f),
	Vec<float>(0.0f, 0.0f, 1.0f),
	Vec<float>(1.0f, 1.0f, 0.0f),
	Vec<float>(0.0f, 1.0f, 1.0f),
	Vec<float>(1.0f, 0.0f, 1.0f)
};

void createStaticVolumeShaderText(std::string strChans);


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

	setShader("DefaultMeshShaders.fx","DefaultMeshPixelShader","");
}

SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn, Vec<float> colorIn) : Material(rendererIn)
{
	colorBuffer.color = DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z,1);
	colorBuffer.colorModifier = DirectX::XMFLOAT4(1,1,1,1);

	renderer->createConstantBuffer(sizeof(ColorBuffer), &constBuffer);

	setShader("DefaultMeshShaders.fx","DefaultMeshPixelShader","");
}

SingleColoredMaterial::SingleColoredMaterial(Renderer* rendererIn, Vec<float> colorIn, float alpha) : Material(rendererIn)
{
	colorBuffer.color = DirectX::XMFLOAT4(colorIn.x,colorIn.y,colorIn.z,alpha);
	colorBuffer.colorModifier = DirectX::XMFLOAT4(1,1,1,1);

	renderer->createConstantBuffer(sizeof(ColorBuffer), &constBuffer);

	setShader("DefaultMeshShaders.fx","DefaultMeshPixelShader","");
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
														 unsigned char* shaderConstMemoryIn/*=NULL*/) : Material(rendererIn)
{
	numChannels = numChannelsIn;
	dims = dimsIn;
	shaderConstMemory = shaderConstMemoryIn;

	cullBackFace = false;
	testDepth = false;
	wireframe = false;

	char cBuffer[3];
	sprintf_s(cBuffer,"%d",numChannels);
	std::string strChans = cBuffer;
	createStaticVolumeShaderText(strChans);

	volPixShaderConsts.channelColors.resize(numChannels);
	volPixShaderConsts.ranges.resize(numChannels);
	volPixShaderConsts.transferFunctions.resize(numChannels);

	if (shaderConstMemory==NULL)
		shaderConstMemory = new unsigned char[volPixShaderConsts.sizeOf()];

	renderer->createConstantBuffer(volPixShaderConsts.sizeOf(),&constBuffer);

	setShader("StaticVolumePixelShader.fx","MultiChanVolumePixelShader",strChans);

	samplerState.resize(numChannels);
	shaderResourceView.resize(numChannels);

	for (int i=0; i<numChannels; ++i)
	{
		samplerState[i] = renderer->getSamplerState();
		shaderResourceView[i] = renderer->createTextureResourceView(dims, image + dims.product()*i);
		setTransferFunction(i,Vec<float>(0.0f,1.0f,0.0f));
		setRange(i,Vec<float>(0.0f,1.0f,0.0f));
		setColor(i,colors[i%6],1.0f);
	}
}

StaticVolumeTextureMaterial::~StaticVolumeTextureMaterial()
{
	for (int i=0; i<numChannels; ++i)
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
	float isOn = (on==true) ? (1.0f) : (0.0f);
	//3 float4s along with 3 times numchannel float4s behind
	size_t memStart = 12*sizeof(float)+12*sizeof(float)*numChannels;

	memcpy((void*)(shaderConstMemory+memStart),&isOn,sizeof(float));
}

void StaticVolumeTextureMaterial::setGradientSampleDir(Vec<float> xDir, Vec<float> yDir, Vec<float> zDir)
{
	//3 times numchannel float4s behind
	size_t memStart = 12*sizeof(float)*numChannels;

	memcpy((void*)(shaderConstMemory+memStart),&xDir,sizeof(float)*3);//TODO can I do this?
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
	renderer->setPixelShaderResourceViews(0,shaderResourceView.size(),shaderResourceView.data());
	renderer->setPixelShaderTextureSamplers(0,samplerState.size(),samplerState.data());
}

void createStaticVolumeShaderText(std::string strChans)
{
	std::string shaderText = "";

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
	shaderText += "	float4 color : SV_TARGET;\n";
	shaderText += "	float depth : SV_DEPTH;\n";
	shaderText += "};\n";
	shaderText += "\n";
	shaderText += "Texture3D    g_txDiffuse[" + strChans + "] : register( t0 );\n";
	shaderText += "SamplerState g_samLinear[" + strChans + "] : register( s0 );\n";
	shaderText += "\n";
	shaderText += "struct VS_OUTPUT\n";
	shaderText += "{\n";
	shaderText += "	float4 Pos : SV_POSITION;\n";
	shaderText += "	float3 TextureUV : TEXCOORD0;\n";
	shaderText += "	float3 Normal : NORMAL;\n";
	shaderText += "};\n";
	shaderText += "\n";
	shaderText += "PixelOutputType MultiChanVolumePixelShader( VS_OUTPUT input ) : SV_TARGET\n";
	shaderText += "{\n";
	shaderText += "\tPixelOutputType output = (PixelOutputType)0;\n";
	shaderText += "\tfloat alpha = 0.0f;\n";
	shaderText += "\n";
	shaderText += "\tfloat4 mainLightDir = float4(-0.5774,-0.5774,0.5774,0);\n";
	shaderText += "\n";
	shaderText += "\t[unroll(" + strChans + ")] for (int i=0; i<" + strChans + "; ++i)\n";
	shaderText += "\t{\n";
	shaderText += "\t\tfloat intensity = g_txDiffuse[i].Sample( g_samLinear[i], input.TextureUV );\n";
	shaderText += "\n";
	shaderText += "\t\tintensity = clamp(intensity,ranges[i][0],ranges[i][1]);\n";
	shaderText += "\t\tintensity = transferFunctions[i][0]*intensity*intensity+transferFunctions[i][1]*intensity+transferFunctions[i][2];\n";
	shaderText += "\n";
	shaderText += "\t\talpha = max(alpha,intensity*channelColor[i].a);\n";
	shaderText += "\n";
	shaderText += "\t\tfloat3 grad;\n";
	shaderText += "\t\tif(lightOn.x>0);\n";
	shaderText += "\t\t{\n";
	shaderText += "\t\t\tgrad.x = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[0]) - \n";
	shaderText += "\t\t\t\tg_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[0]);\n";
	shaderText += "\t\t\tgrad.y = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[1]) - \n";
	shaderText += "\t\t\t\tg_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[1]);\n";
	shaderText += "\t\t\tgrad.z = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[2]) - \n";
	shaderText += "\t\t\t\tg_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[2]);\n";
	shaderText += "\n";
	shaderText += "\t\t\tgrad = normalize(grad);\n";
	shaderText += "\n";
	shaderText += "\t\t\tintensity *= saturate(dot(grad,mainLightDir))*0.7 + 0.3;\n";
	shaderText += "\t\t}\n";
	shaderText += "\t\toutput.color += intensity*channelColor[i];\n";
	shaderText += "\t}\n";
	shaderText += "\n";
	shaderText += "\toutput.color.a = saturate(alpha);\n";
	shaderText += "\toutput.color.a = output.color.a*step(0.1,output.color.a);\n";
	//shaderText += "\toutput.color.rgb = saturate(input.TextureUV);\n";//////////////////////////////////////////
	//shaderText += "\toutput.color.a = 1.0f;\n";///////////////
	shaderText += "\n";
	shaderText += "\toutput.depth = input.Pos.z;\n";
	shaderText += "\n";
	shaderText += "\treturn output;\n";
	shaderText += "}\n";

	FILE* file;
	fopen_s(&file,"StaticVolumePixelShader.fx","w");

	fprintf(file,shaderText.c_str());

	fclose(file);
}
