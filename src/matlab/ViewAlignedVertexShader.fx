cbuffer VSConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 TextureUV : TEXCOORD0;
	float3 Normal : NORMAL;
	float4 clipPlane[2] : SV_ClipDistance;
};

VS_OUTPUT ViewAlignedVertexShader( float4 Pos : POSITION,  float3 TextureUV : TEXCOORD, float3 Normal : NORMAL )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.TextureUV = mul(float4(TextureUV,1), World).xyz; 

	output.Pos = mul( Pos, View );
	output.Pos = mul( output.Pos, Projection );

	output.clipPlane[0] = float4(output.TextureUV.x, output.TextureUV.y, output.TextureUV.z,0.5f);
	output.clipPlane[1] = float4(1.0f-output.TextureUV.x, 1.0f-output.TextureUV.y, 1.0f-output.TextureUV.z,0.5f);

	return output;
}