#include "DragDropButton.h"
#include "GeometryGenerator.h"
namespace hlab {

DragDropButton::DragDropButton(ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context,
    const std::vector<MeshData>& meshes) {
    this->Initialize(device, context, meshes);
    }

 void DragDropButton::Initialize(ComPtr<ID3D11Device> &device,
                                ComPtr<ID3D11DeviceContext> &context,
                                const std::vector<MeshData> &meshes) 
{
    Model::Initialize(device, context, meshes);
}

 void DragDropButton::CreateConstBuffers(ComPtr<ID3D11Device> &device,
                                        int size) {
    m_buttonConstsCPU.reserve(size);
    m_buttonConstsGPU.reserve(size);
    for (int i = 0; i<size ;i++) {
        UIButtonConstants ConstsCPU;
        ComPtr<ID3D11Buffer> ConstsGPU;
        m_buttonConstsCPU.push_back(ConstsCPU);
        m_buttonConstsGPU.push_back(ConstsGPU);
        D3D11Utils::CreateConstBuffer(device, m_buttonConstsCPU[i],
                                      m_buttonConstsGPU[i]);
    }
 }

void DragDropButton::UpdateConstantBuffers(
    ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {
    Model::UpdateConstantBuffers(device, context);
    if (m_isVisible) {
        if (m_buttonConstsCPU.size() != m_buttonConstsGPU.size()) {
            return;
        }
        for (int i = 0; i < m_buttonConstsCPU.size(); i++) {
            D3D11Utils::UpdateBuffer(device, context, m_buttonConstsCPU[i],
                                     m_buttonConstsGPU[i]);
        }
        
    }
}

int DragDropButton::GetCursorInButtonIndex(float& CursorX, float& CursorY) {
    
    for (int i = 0; i < m_buttonConstsCPU.size(); i++) {
        float m_minX = m_buttonConstsCPU[i].screenPos.x -
                       m_buttonConstsCPU[i].buttonLength / 2.0f;
        float m_maxX = m_buttonConstsCPU[i].screenPos.x +
                       m_buttonConstsCPU[i].buttonLength / 2.0f;
        float m_minY = m_buttonConstsCPU[i].screenPos.y -
                       m_buttonConstsCPU[i].buttonLength / 2.0f;
        float m_maxY = m_buttonConstsCPU[i].screenPos.y +
                       m_buttonConstsCPU[i].buttonLength / 2.0f;

            if (CursorX >= m_minX && CursorX <= m_maxX && CursorY >= m_minY &&
            CursorY <= m_maxY) {
                return i;
            }
    }
    return -1;
}


void DragDropButton::Render(ComPtr<ID3D11DeviceContext> &context) {
        if (m_isVisible) {
        //Mesh는 일단 하나로 고정,,ㅎ
        const auto &mesh = m_meshes[0];
            for (const auto &buttonConst : m_buttonConstsGPU) {
            context->VSSetConstantBuffers(
                0, 1, mesh->vertexConstBuffer.GetAddressOf());
            context->VSSetConstantBuffers(2, 1,
                                          buttonConst.GetAddressOf());
            context->PSSetConstantBuffers(
                0, 1, mesh->pixelConstBuffer.GetAddressOf());

            context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf());

            // 물체 렌더링할 때 여러가지 텍스춰 사용 (t0 부터시작)
            vector<ID3D11ShaderResourceView *> resViews = {
                mesh->albedoSRV.Get(), mesh->normalSRV.Get(), mesh->aoSRV.Get(),
                mesh->metallicRoughnessSRV.Get(), mesh->emissiveSRV.Get()};
            context->PSSetShaderResources(0, UINT(resViews.size()),
                                          resViews.data());

            context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
                                        &mesh->stride, &mesh->offset);

            context->IASetIndexBuffer(mesh->indexBuffer.Get(),
                                      DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(mesh->indexCount, 0, 0);
        }
    }
}

}
