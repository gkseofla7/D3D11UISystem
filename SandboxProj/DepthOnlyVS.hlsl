#include "Common.hlsli" // ���̴������� include ��� ����

cbuffer MeshConstants : register(b0)
{
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

cbuffer ActorConstants : register(b10)
{
    matrix world; // Model(�Ǵ� Object) ��ǥ�� -> World�� ��ȯ
    matrix worldIT;
    float3 indexColor;
    float dummy3;
};

float4 main(VertexShaderInput input) : SV_POSITION
{
    float4 pos = mul(float4(input.posModel, 1.0f), world);
    float4 output = mul(pos, viewProj);
    
    return output;
}
