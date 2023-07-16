#include "MultipleModel.h"

namespace hlab {

	MultipleModel::MultipleModel(ComPtr<ID3D11Device> &device,
                             ComPtr<ID3D11DeviceContext> &context,
                             const std::string &basePath,
                             const std::string &filename) {
        this->Initialize(device, context, basePath, filename);
    }
    MultipleModel::MultipleModel(ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context,
        const std::vector<MeshData>& meshes) {
        this->Initialize(device, context, meshes);
    }

    void MultipleModel::AddMeshConstBuffers(ComPtr<ID3D11Device> &device) {
        MeshConstants meshConstsCPU;
        ComPtr<ID3D11Buffer> meshConstsGPU;
        m_meshConstsCPUs.push_back(meshConstsCPU);
        m_meshConstsGPUs.push_back(meshConstsGPU);

        D3D11Utils::CreateConstBuffer(device, meshConstsCPU, meshConstsGPU);
    }

}


