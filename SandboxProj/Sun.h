#pragma once
#include "BillboardPoints.h"
namespace hlab {
class Sun {

    void Render(ComPtr<ID3D11DeviceContext> &context);
    void UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                               ComPtr<ID3D11DeviceContext> &context);

  public:
    shared_ptr<BillboardPoints> m_model;
};

} // namespace hlab
