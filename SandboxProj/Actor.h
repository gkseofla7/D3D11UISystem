#pragma once
#include "Model.h"

namespace hlab {

class Actor {
  public:
    Actor(){}
    Actor(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
          shared_ptr<Model> &model) {
        Initialize(device, context, model);
    }
    virtual void Initialize(ComPtr<ID3D11Device> &device,
                            ComPtr<ID3D11DeviceContext> &context,
                            shared_ptr<Model> &model);
    virtual void UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                               ComPtr<ID3D11DeviceContext> &context);

    virtual void Render(ComPtr<ID3D11DeviceContext> &context); 
  public:
   shared_ptr<Model> m_model;
  ActorConstants m_actorConstsCPU;
   bool m_isVisible = true;

private:
  ComPtr < ID3D11Buffer > m_actorConstsGPU;
};



}
