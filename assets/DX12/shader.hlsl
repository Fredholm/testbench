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

cbuffer TranslationBlock : register(b0)
{
    matrix tx;
};

PSInput VSMain(VS_INPUT input)
{
	PSInput result;

    result.position = input.position;
    result.normal = input.normal;
    result.tex = input.tex;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return float4(1, 1, 1, 1);
}