Texture2D<float4> Texture : register (t0, space2);
SamplerState Sampler : register (s0, space2);

struct PS_INPUT
{
	float2 Uv : TEXCOORD0;
	float4 Color : TEXCOORD1;
};

float4 main (PS_INPUT input) : SV_TARGET0
{	
	return Texture.Sample (Sampler, input.Uv) * input.Color;
}
