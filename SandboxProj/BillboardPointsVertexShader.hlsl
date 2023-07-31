#include "Common.hlsli" // 쉐이더에서도 include 사용 가능
cbuffer BillboardPointsConstantData : register(b0)
{
    float width;
    float time;
    float3 cameraUpDir;
    float3 padding;
};

struct BillboardVertexShaderInput
{
    float4 pos : POSITION; // 모델 좌표계의 위치 position
};

struct GeometryShaderInput
{
    float4 pos : SV_POSITION; // Screen position
};

GeometryShaderInput main(BillboardVertexShaderInput input)
{
    GeometryShaderInput output;
    
    // Geometry shader로 그대로 넘겨줍니다.
    output.pos = input.pos;
    
    return output;
}