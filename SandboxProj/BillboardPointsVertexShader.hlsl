#include "Common.hlsli" // ���̴������� include ��� ����
cbuffer BillboardPointsConstantData : register(b0)
{
    float width;
    float time;
    matrix world;
    float2 padding;
};

struct BillboardVertexShaderInput
{
    float4 pos : POSITION; // �� ��ǥ���� ��ġ position
};

struct GeometryShaderInput
{
    float4 pos : SV_POSITION; // Screen position
};

GeometryShaderInput main(BillboardVertexShaderInput input)
{
    GeometryShaderInput output;
    
    output.pos = mul(input.pos, world);
    return output;
}