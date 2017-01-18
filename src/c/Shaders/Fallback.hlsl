cbuffer VSConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;

	float4 depthPeelPlanes;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 TextureUV : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct PS_OUTPUT
{
	float4 color : SV_TARGET;
	float depth : SV_DEPTH;
};

VS_OUTPUT FallbackVertexShader( float4 Pos : POSITION,  float3 TextureUV : TEXCOORD, float3 Normal : NORMAL )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.TextureUV = TextureUV;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	output.Normal = mul( Normal, World );

	return output;
}

PS_OUTPUT FallbackPixelShader( VS_OUTPUT input )
{
	PS_OUTPUT output;

	float4 mainLightDir = float4(-0.5774,-0.5774,0.5774,0);

	float pxChkX = fmod(floor(input.Pos.x / 10.0f), 2.0f);
	float pxChkY = fmod(floor(input.Pos.y / 10.0f), 2.0f);

	float3 color = abs(pxChkX-pxChkY) * float3(1.0f, 0.1f, 0.1f);

	float lightInt = 0.7*dot(-input.Normal,mainLightDir);
	lightInt = saturate(lightInt) + 0.3;

	output.color = float4(color * lightInt, 1.0f);
	output.depth = input.Pos.z;

	return output;
}
