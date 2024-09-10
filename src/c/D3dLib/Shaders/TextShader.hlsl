cbuffer VSConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 depthPeelPlanes;
};

cbuffer PSConstantBuffer : register( b1 )
{
	float4 flags;
};

Texture2D    textAtlas : register( t0 );
SamplerState textSampler : register( s0 );

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexUV : TEXCOORD;
	float4 FgColor : COLOR0;
	float4 BgColor : COLOR1;
};

VS_OUTPUT TextQuadVS_PTCC( float4 Pos : POSITION, float3 TexUV : TEXCOORD, float4 FgColor : COLOR0, float4 BgColor : COLOR1 )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	output.TexUV = TexUV.xy;
	output.FgColor = FgColor;
	output.BgColor = BgColor;

	return output;
}

float4 TextQuadPS( VS_OUTPUT input ) : SV_TARGET
{
	float alpha = textAtlas.Sample(textSampler, input.TexUV);

	// Just alpha-blend foreground and background colors
	return alpha * input.FgColor + (1.0f-alpha) * input.BgColor;
}
