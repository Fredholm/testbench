#define POSITION 0
#define NORMAL 1
#define TEXTCOORD 2
#define TRANSLATION 5
#define TRANSLATION_NAME TranslationBlock
#define DIFFUSE_TINT 6
#define DIFFUSE_TINT_NAME DiffuseColor
StructuredBuffer<float4> pos : register(t0);
StructuredBuffer<float4> nor : register(t1);
StructuredBuffer<float2> tex : register(t2);

cbuffer SceneConstantBuffer : register(b5)
{
    float4 tx;
};

struct PSInput
{
	float4 position : SV_POSITION;
    float4 normal : SECOND;
    float2 tex : THIRD;
};

PSInput VSMain(uint vertexID : SV_VertexID)
{
	PSInput result;

	result.position = pos[vertexID] + float4(tx.x, tx.y, -tx.z, tx.w);
    result.normal   = nor[vertexID];
    result.tex      = tex[vertexID];

	return result;
}