#pragma once
#include "Model.h"


namespace hlab {


class ButtonInfo {

public:
    Vector2 m_buttonScreenPos;
    float m_length;
};


class DragDropButton : public Model 
{
  public:
    DragDropButton(ComPtr<ID3D11Device> &device,
                   ComPtr<ID3D11DeviceContext> &context,
          const std::vector<MeshData> &meshes);

    virtual void Initialize(ComPtr<ID3D11Device> &device,
                            ComPtr<ID3D11DeviceContext> &context,
                            const std::vector<MeshData> &meshes) override;
    virtual void UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                          ComPtr<ID3D11DeviceContext> &context) override;
    void CreateConstBuffers(ComPtr<ID3D11Device> &device, int size);
    int GetCursorInButtonIndex(float &CursorX, float &CursorY);
    
    virtual void Render(ComPtr<ID3D11DeviceContext> &context) override; 
 public:
    vector<UIButtonConstants> m_buttonConstsCPU;

 private:
    vector<ComPtr<ID3D11Buffer>> m_buttonConstsGPU;
};


}
