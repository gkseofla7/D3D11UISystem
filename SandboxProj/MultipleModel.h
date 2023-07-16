#pragma once
#include "Model.h"
namespace hlab {

class MultipleModel : public Model {
  public:
    MultipleModel() {}
    MultipleModel(ComPtr<ID3D11Device> &device,
                  ComPtr<ID3D11DeviceContext> &context,
          const std::string &basePath, const std::string &filename);
    MultipleModel(ComPtr<ID3D11Device> &device,
                  ComPtr<ID3D11DeviceContext> &context,
          const std::vector<MeshData> &meshes);

    void AddMeshConstBuffers(ComPtr<ID3D11Device> &device);

public:
    vector<MeshConstants> m_meshConstsCPUs;

private:
    vector<ComPtr<ID3D11Buffer>> m_meshConstsGPUs;
	
};

}

