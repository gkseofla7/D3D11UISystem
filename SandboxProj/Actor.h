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
    void UpdateWorldRow(const Matrix &worldRow);
    virtual void Render(ComPtr<ID3D11DeviceContext> &context); 
    void RenderNormals(ComPtr<ID3D11DeviceContext> &context);
    template <typename T> T GetBoundingVolume() {
        if (m_boundingType == ModelBoundingType::SPHERE) {
            return m_boundingSphere;
        } else if (m_boundingType == ModelBoundingType::BOX) {
            return m_boundingBox;
        } else {
            return nullptr;
        }
    }
  public:
   shared_ptr<Model> m_model;
  //MeshConstants ¿Ãµø

  ActorConstants m_actorConstsCPU;
  bool m_isVisible = true;
  bool m_castShadow = true;
  bool m_drawNormals = false;

  DirectX::BoundingSphere m_boundingSphere = DirectX::BoundingSphere();
  DirectX::BoundingBox m_boundingBox = DirectX::BoundingBox();
  ModelBoundingType m_boundingType = ModelBoundingType::None;
  Matrix m_worldMatrix;

private:
  ComPtr < ID3D11Buffer > m_actorConstsGPU;

};



}
