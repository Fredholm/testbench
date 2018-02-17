#define POSITION 0
#define NORMAL 1
#define TEXTCOORD 2
#define TRANSLATION 5
#define TRANSLATION_NAME TranslationBlock
#define DIFFUSE_TINT 6
#define DIFFUSE_TINT_NAME DiffuseColor
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
#ifdef DIFFUSE_SLOT
	float4 col = g_texture.Sample(g_sampler, input.tex);
#else
	float4 col = float4(1.0,1.0,0.0, 1.0);
#endif

	return col * diffuse;
}