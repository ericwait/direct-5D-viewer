///////////////////////////////////////////////////////////////////////////////
// Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
// This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
// tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
// for details. LEVER 3-D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
// LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with
// LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////

cbuffer ConstantPSBuffer : register( b1 )
{
	float4 transferFunctions[3];
	float4 ranges[3];
	float4 channelColor[3];
	float4 gradientSampleDirection[3];
	float4 lightOn;
};

struct PixelOutputType
{
	float4 color : SV_TARGET;
	float depth : SV_DEPTH;
};

Texture3D    g_txDiffuse[3] : register( t0 );
SamplerState g_samLinear[3] : register( s0 );

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 TextureUV : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Dpth : TEXCOORD1;
};

PixelOutputType MultiChanVolumePixelShader( VS_OUTPUT input )
{
	PixelOutputType output = (PixelOutputType)0;
	float alpha = 0.0f;

	float4 mainLightDir = float4(-0.5774,-0.5774,0.5774,0);
	float3 unlitComposite = float3(0.0f,0.0f,0.0f);
	float3 alphaComposite = float3(0.0f,0.0f,0.0f);

	int numAlpha = 0;
	float3 grad;

float maxIntensity = 0;

	[unroll(3)] for (int i=0; i<3; ++i)
	{
		float intensity = g_txDiffuse[i].Sample( g_samLinear[i], input.TextureUV );
		intensity = clamp(intensity,ranges[i][0],ranges[i][1]);
		intensity = transferFunctions[i][0]*intensity*intensity+transferFunctions[i][1]*intensity+transferFunctions[i][2];
		if (intensity < 0.2f || channelColor[i].a < 0.01f)
		{
			intensity = 0;
		}
		maxIntensity = max(intensity,maxIntensity);

		float lightMod = 1.0f;
		if(lightOn.x>0)
		{
			grad.x = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[0]) - 
				g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[0]);
			grad.y = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[1]) - 
				g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[1]);
			grad.z = g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV+gradientSampleDirection[2]) - 
				g_txDiffuse[i].Sample(g_samLinear[i], input.TextureUV-gradientSampleDirection[2]);
			grad = normalize(grad);
			float ambientLight = 0.45f;
			lightMod = saturate(dot(grad,mainLightDir)*2.0*(1-ambientLight) + ambientLight);
		}

		output.color.rgb += (lightMod*intensity*channelColor[i].rgb);
		unlitComposite += intensity*channelColor[i].rgb;
		alphaComposite += intensity*channelColor[i].rgb*channelColor[i].a;
	}

	float maxComponent = max(max(unlitComposite.r,unlitComposite.g),unlitComposite.b);
	float maxAplha = max(max(alphaComposite.r,alphaComposite.g),alphaComposite.b);
	if (maxComponent!=0)
	{
		output.color.rgb /= maxComponent;
		output.color.rgb *= maxIntensity;
	}
	if (maxAplha!=0)
	{
		output.color.a = maxAplha;
	}else{
		output.color.a = 0;
	}


	if(lightOn.y>0)
	{
		float distMult = (input.Dpth.z<=0) ? (1) : (1-input.Dpth.z);
		output.color.r *= distMult;
		output.color.g *= distMult;
		output.color.b *= distMult;
		output.color.a *= distMult;
	}

	output.depth = 1.0f;

	return output;
}
