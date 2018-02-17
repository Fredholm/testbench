#define T(slot) t##slot
#define B(slot) b##slot

StructuredBuffer<float4> pos : register(T(POSITION));
StructuredBuffer<float4> nor : register(T(NORMAL));
StructuredBuffer<float2> tex : register(T(TEXTCOORD));

cbuffer SceneConstantBuffer : register(B(TRANSLATION))
{
    float4 tx;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 normal : _NORMAL;
	float2 tex : _TEXTCOORD;
};

PSInput VSMain(uint vertexID : SV_VertexID)
{
	PSInput result;

	result.position = pos[vertexID] + float4(tx.x, tx.y, -tx.z, tx.w);
    result.normal   = nor[vertexID];
    result.tex      = tex[vertexID];

	return result;
}