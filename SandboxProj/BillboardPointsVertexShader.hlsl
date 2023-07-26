cbuffer BillboardPointsConstantData : register(b0)
{
    float3 eyeWorld; // For geometry shader
    float width; // For geometry shader
    Matrix model; // For vertex shader
    Matrix view; // For vertex shader
    Matrix proj; // For vertex shader
};

struct VertexShaderInput
{
    float4 pos : POSITION; // 모델 좌표계의 위치 position
};

struct GeometryShaderInput
{
    float4 pos : SV_POSITION; // Screen position
};

GeometryShaderInput main(VertexShaderInput input)
{
    GeometryShaderInput output;
    
    // Geometry shader로 그대로 넘겨줍니다.
    output.pos = input.pos;
    
    return output;
}