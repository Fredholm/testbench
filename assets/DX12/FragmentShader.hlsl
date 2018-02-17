#define T(slot) t##slot
#define B(slot) b##slot

cbuffer SceneDiffuseColor : register(B(DIFFUSE_TINT))
{
	float4 diffuse;
};

#ifdef DIFFUSE_SLOT
Texture2D g_texture : register(T(DIFFUSE_SLOT));
#endif
SamplerState g_sampler : register(s0);

struct PSInput
{
	float4 position : SV_POSITION;
	float4 normal : _NORMAL;
	float2 tex : _TEXTCOORD;
};

float4 PSMain(PSInput input) : SV_TARGET
{
#ifdef DIFFUSE_SLOT
	float4 col = g_texture.Sample(g_sampler, input.tex);
#else
	float4 col = float4(1.0,1.0,1.0, 1.0);
#endif

	return col * diffuse;
}