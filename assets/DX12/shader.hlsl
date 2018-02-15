
StructuredBuffer<float4> position : register(POSITION);
StructuredBuffer<float3> normal : register(t1);
StructuredBuffer<float2> uv : register(t2);

struct cb_t
{
	float4 v;
};
ConstantBuffer<cb_t> tx : register(b5);
ConstantBuffer<cb_t> color : register(b6);

Texture2D g_texture : register(t7);
SamplerState g_sampler : register(s0);


struct Pixel
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};


Pixel VSMain(uint vid : SV_VertexId)
{
	Pixel result;

    result.position = position[vid] + tx.v;
    result.normal   = normal[vid];
    result.uv       = uv[vid];

	return result;
}

float4 PSMain(Pixel input) : SV_TARGET
{
    float3 texColor = g_texture.Sample(g_sampler, input.uv).rgb + color.v.rgb;
    return float4(texColor, 1.f);
}