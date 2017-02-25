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
	float3 Normal : NORMAL;
};

struct PS_OUTPUT
{
	float4 color : SV_TARGET;
	float depth : SV_DEPTH;
};

VS_OUTPUT FallbackVS_P( float4 Pos : POSITION )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	return output;
}

VS_OUTPUT FallbackVS_PT( float4 Pos : POSITION, float3 TexUV : TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	return output;
}

VS_OUTPUT FallbackVS_PTC( float4 Pos : POSITION, float3 TexUV : TEXCOORD, float4 Color : COLOR0 )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	return output;
}

VS_OUTPUT FallbackVS_PTCC( float4 Pos : POSITION, float3 TexUV : TEXCOORD, float4 Color : COLOR0, float4 ColorBack : COLOR1 )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	return output;
}


VS_OUTPUT FallbackVS_PN( float4 Pos : POSITION, float3 Normal : NORMAL )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	output.Normal = mul( Normal, World );

	return output;
}

VS_OUTPUT FallbackVS_PNT( float4 Pos : POSITION, float3 Normal : NORMAL, float3 TexUV : TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	output.Normal = mul( Normal, World );

	return output;
}

VS_OUTPUT FallbackVS_PNC( float4 Pos : POSITION, float3 Normal : NORMAL, float4 Color : COLOR0 )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	output.Normal = mul( Normal, World );

	return output;
}

VS_OUTPUT FallbackVS_PNTC( float4 Pos : POSITION, float3 Normal : NORMAL, float3 TexUV : TEXCOORD, float4 Color : COLOR0 )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	output.Normal = mul( Normal, World );

	return output;
}

VS_OUTPUT FallbackVS_PNTCC( float4 Pos : POSITION, float3 Normal : NORMAL, float3 TexUV : TEXCOORD, float4 Color : COLOR0, float4 ColorBack : COLOR1 )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	output.Normal = mul( Normal, World );

	return output;
}


PS_OUTPUT FallbackPS( VS_OUTPUT input )
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
