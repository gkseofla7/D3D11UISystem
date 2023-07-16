#include "DragDropButton.h"
#include "GeometryGenerator.h"
namespace hlab {


void DragDropButton::UpdateConstantBuffers(
    ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {
    D3D11Utils::UpdateBuffer(device, context, m_buttonConstsCPU,
                             m_buttonConstsGPU);      
    Actor::UpdateConstantBuffers(device, context);
}
bool DragDropButton::IsCursorInButton(float& CursorX, float& CursorY) {
   float m_minX =
        m_buttonConstsCPU.screenPos.x - m_buttonConstsCPU.buttonLength / 2.0f;
    float m_maxX =
        m_buttonConstsCPU.screenPos.x + m_buttonConstsCPU.buttonLength / 2.0f;
    float m_minY =
        m_buttonConstsCPU.screenPos.y - m_buttonConstsCPU.buttonLength / 2.0f;
    float m_maxY =
        m_buttonConstsCPU.screenPos.y + m_buttonConstsCPU.buttonLength / 2.0f;

    if (CursorX >= m_minX && CursorX <= m_maxX && CursorY >= m_minY &&
        CursorY <= m_maxY) {
        return true;
    }
    
    return false;
}


void DragDropButton::Render(ComPtr<ID3D11DeviceContext> &context) {
    context->VSSetConstantBuffers(2, 1, m_buttonConstsGPU.GetAddressOf());
    Actor::Render(context);
}

}
