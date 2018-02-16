StructuredBuffer<float4> pos : register(t0);
StructuredBuffer<float4> nor : register(t1);
StructuredBuffer<float2> tex : register(t2);

cbuffer SceneConstantBuffer : register(b5)
{
    float4 tx;
};

Texture2D g_texture : register(t7);
SamplerState g_sampler : register(s0);

struct PSInput
{
	float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 tex : TEXCOORD;
};

PSInput VSMain(uint vertexID : SV_VertexID)
{
	PSInput result;

	result.position = pos[vertexID] + float4(tx.x, tx.y, -tx.z, tx.w);
    result.normal   = nor[vertexID];
    result.tex      = tex[vertexID];

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 texColor = g_texture.Sample(g_sampler, input.tex);
    return float4(texColor, 1.f);
}