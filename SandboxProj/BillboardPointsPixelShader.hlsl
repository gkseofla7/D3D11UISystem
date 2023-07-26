Texture2DArray g_texArray : register(t0);
SamplerState g_sampler : register(s0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION; // Screen position
    float4 posWorld : POSITION0;
    float4 center : POSITION1;
    float2 texCoord : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(PixelShaderInput input)
{
    float3 uvw = float3(input.texCoord, float(input.primID % 5));
    float4 pixelColor = g_texArray.Sample(g_sampler, uvw);

    // clip(x)에서 x가 0보다 작으면 이 픽셀의 색은 버린다.     
    
    // alpha 값이 있는 이미지에서 불투명도가 0.9보다 작으면 clip
    clip(pixelColor.a - 0.9f);
    
    // 픽셀의 값이 흰색에 가까운 배경 색이면 clip
    // 조건 연산자 사용
    clip((pixelColor.r + pixelColor.g + pixelColor.b) > 0.8 * 3.0 ? -1 : 1);
    
    // 픽셀의 값이 흰색에 가까운 배경 색이면 clip
    //TODO: clip(...)
    //float i = (pixelColor.r + pixelColor.g + pixelColor.b) / 3.0;
    //clip((i > 0.8) ? -1 : 1);
    
    //clip((pixelColor.a < 0.9f) || (pixelColor.r + pixelColor.g + pixelColor.b) > 2.4 ? -1 : 1);
    
    PixelShaderOutput output;
    
    output.pixelColor = pixelColor;

    return output;
}
