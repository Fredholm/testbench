cbuffer SceneDiffuseColor : register(b6)
{
	float4 diffuse;
};

Texture2D g_texture : register(t7);
SamplerState g_sampler : register(s0);

struct PSInput
{
	float4 position : SV_POSITION;
    float4 normal : SECOND;
    float2 tex : THIRD;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 texColor = g_texture.Sample(g_sampler, input.tex) + diffuse.rgb;
    return float4(texColor, 1.f);
}