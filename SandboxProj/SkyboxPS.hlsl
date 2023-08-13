#include "Common.hlsli" // ���̴������� include ��� ����

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

    //�¾��� ��ġ�� ���� ��� �� ����
    //World Position
    //���� ��ġ�� �¾� �ֺ��� �ִٸ�~
    float4 lightPosProj = mul(float4(input.posModel, 1.0), lights[LIGHT_SUN].position);
    lightPosProj = mul(lightPosProj, proj);
    
    //�� ����������
    if (length(lightPosProj - input.posProj) < 50.0)
    {
        output.pixelColor.r *= 3.;

    }
    
    output.pixelColor *= strengthIBL;
    
    return output;
}
