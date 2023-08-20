#pragma once
#include <directxtk/SimpleMath.h>
#include "Actor.h"
namespace hlab {
class Mirror : public Actor {
  public:
    Mirror() {}
    Mirror(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
          shared_ptr<Model> &model) {
        Initialize(device, context, model);
    }
    DirectX::SimpleMath::Plane m_mirrorPlane;
};
} // namespace hlab