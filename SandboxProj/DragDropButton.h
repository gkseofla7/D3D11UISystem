#pragma once
#include "Actor.h"
#include "Model.h"

namespace hlab {

class DragDropButton : public Actor 
{
  public:
    DragDropButton(ComPtr<ID3D11Device> &device,
                   ComPtr<ID3D11DeviceContext> &context,
          shared_ptr<Model> &model) {
        Actor::Initialize(device, context, model);
        D3D11Utils::CreateConstBuffer(device, m_buttonConstsCPU,
                                      m_buttonConstsGPU);
    }
    bool IsCursorInButton(float &CursorX, float &CursorY);
    virtual void
    UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                          ComPtr<ID3D11DeviceContext> &context) override;
    virtual void Render(ComPtr<ID3D11DeviceContext> &context) override; 
 public:
    UIButtonConstants m_buttonConstsCPU;
   shared_ptr<Model> m_createModel;
 private:
    ComPtr<ID3D11Buffer> m_buttonConstsGPU;
};


}
