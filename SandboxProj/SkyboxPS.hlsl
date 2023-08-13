#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

struct SkyboxPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(SkyboxPixelShaderInput input)
{
    PixelShaderOutput output;
    
    if (textureToDraw == 0)
        output.pixelColor = envIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    else if (textureToDraw == 1)
        output.pixelColor = specularIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    else if (textureToDraw == 2)
        output.pixelColor = irradianceIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    else
        output.pixelColor = float4(135/255, 206/255, 235/255, 1);

    //태양의 위치에 따라 배경 색 결정
    //World Position
    //현재 위치가 태양 주변에 있다면~
    float4 lightPosProj = mul(float4(input.posModel, 1.0), lights[LIGHT_SUN].position);
    lightPosProj = mul(lightPosProj, proj);
    
    //좀 내려갈때만
    if (length(lightPosProj - input.posProj) < 50.0)
    {
        output.pixelColor.r *= 3.;

    }
    
    output.pixelColor *= strengthIBL;
    
    return output;
}
