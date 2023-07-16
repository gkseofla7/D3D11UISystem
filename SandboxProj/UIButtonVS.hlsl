#include "Common.hlsli"

cbuffer MeshConstants : register(b2)
{
    float2 screenPos;
    float buttonLength = 0.0f;
    float dummy;
};

struct SamplingVertexShaderInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
    SamplingPixelShaderInput output;
    //DragDropButton은 (0,0) 에 위치
    output.position = float4(input.position, 1.);
    output.position.x = output.position.x+screenPos.x;
    output.position.y = output.position.y+ screenPos.y;
    output.texcoord = input.texcoord;

    return output;
}
