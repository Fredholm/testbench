cbuffer SceneConstantBuffer : register(b0)
{
    float4 tx;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct VS_INPUT
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 tex : TEXCOORD;
};

struct PSInput
{
	float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 tex : TEXCOORD;
};

PSInput VSMain(VS_INPUT input)
{
	PSInput result;

    result.position = input.position + tx;
    result.normal = input.normal;
    result.tex = input.tex;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.tex);
}