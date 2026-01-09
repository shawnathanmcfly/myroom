
struct VS_INPUT
{
	float3 Pos : TEXCOORD0;
	float4 Color : TEXCOORD1;
	float2 Uv : TEXCOORD2;
};

struct VS_OUTPUT
{
	float2 Uv : TEXCOORD0;
	float4 Color : TEXCOORD1;
	float4 Pos : SV_POSITION;
};

cbuffer UniformBlock : register (b0, space1)
{
	float4x4 mvp;
};

VS_OUTPUT main (VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.Pos = mul (mvp, float4 (input.Pos, 1));
	output.Color = input.Color;
	output.Uv = input.Uv;
	
	return output;
}
