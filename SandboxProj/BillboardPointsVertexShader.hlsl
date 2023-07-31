#include "Common.hlsli" // ���̴������� include ��� ����
cbuffer BillboardPointsConstantData : register(b0)
{
    float width;
    float time;
    float3 cameraUpDir;
    float3 padding;
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
    
    // Geometry shader�� �״�� �Ѱ��ݴϴ�.
    output.pos = input.pos;
    
    return output;
}