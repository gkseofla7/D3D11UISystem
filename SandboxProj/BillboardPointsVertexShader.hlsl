#include "Common.hlsli" // ���̴������� include ��� ����
cbuffer BillboardPointsConstantData : register(b0)
{
    float width;
    float time;
    float2 padding;
    matrix world;
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
   // output.pos = input.pos;
    return output;
}