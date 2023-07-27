#include "BillboardPoints.h"

#include <numeric>

namespace hlab {
void BillboardPoints::Initialize(ComPtr<ID3D11Device> &device,
                                 ComPtr<ID3D11DeviceContext> &context,
                                 const std::vector<Vector4> &points, const float width,
                                 std::vector<std::string> filenames) {
    // Sampler 만들기
    // Geometry shader 초기화하기
    D3D11Utils::CreateVertexBuffer(device, points, m_vertexBuffer);
    m_indexCount = uint32_t(points.size());
    m_billboardPointsConstsCPU.width = width;
    D3D11Utils::CreateConstBuffer(device, m_billboardPointsConstsCPU,
                                  m_billboardPointsConstsGPU);

    D3D11Utils::CreateTextureArray(
         device, context, filenames, m_texArray, m_texArraySRV);
}
void BillboardPoints::UpdateConstantBuffers(
    ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {
    D3D11Utils::UpdateBuffer(device, context, m_billboardPointsConstsCPU,
                             m_billboardPointsConstsGPU);
}
void BillboardPoints::Render(ComPtr<ID3D11DeviceContext> &context) {

    //context->VSSetShader(m_vertexShader.Get(), 0, 0);
    //context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
    //context->PSSetShader(m_pixelShader.Get(), 0, 0);
    context->PSSetShaderResources(0, 1, m_texArraySRV.GetAddressOf());
    //context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    context->VSSetConstantBuffers(0, 1,
                                  m_billboardPointsConstsGPU.GetAddressOf());
    context->PSSetConstantBuffers(0, 1,
                                  m_billboardPointsConstsGPU.GetAddressOf());
    // Geometry Shader
    context->GSSetConstantBuffers(0, 1,
                                  m_billboardPointsConstsGPU.GetAddressOf());
    //context->GSSetShader(m_geometryShader.Get(), 0, 0);

    //context->IASetInputLayout(m_inputLayout.Get());

    UINT stride = sizeof(Vector4); // sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride,
                                &offset);
    //context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    // POINTLIST는 연결관계가 필요 없기 때문에 DrawIndexed() 대신 Draw() 사용
    context->Draw(m_indexCount, 0);
}

} // namespace hlab