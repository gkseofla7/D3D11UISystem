#include "Actor.h"

namespace hlab {

    void Actor::Initialize(ComPtr<ID3D11Device> &device,
        ComPtr<ID3D11DeviceContext>& context,
        shared_ptr<Model>& model) {
            m_model = model;
            D3D11Utils::CreateConstBuffer(device, m_actorConstsCPU,
                                      m_actorConstsGPU);

    }


    void Actor::UpdateConstantBuffers(
             ComPtr<ID3D11Device> &device,
             ComPtr<ID3D11DeviceContext> &context) {
            m_model->UpdateConstantBuffers(device, context);
            D3D11Utils::UpdateBuffer(device, context, m_actorConstsCPU,
                                     m_actorConstsGPU);      
    }

    void Actor::Render(ComPtr<ID3D11DeviceContext> &context) { 
        if (m_isVisible) {
                context->VSSetConstantBuffers(10, 1,
                                          m_actorConstsGPU.GetAddressOf());
                m_model->Render(context);
        }
    }
    }