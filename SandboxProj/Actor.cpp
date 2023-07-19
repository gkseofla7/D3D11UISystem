#include "Actor.h"
#include "IndexColorMaker.h"

namespace hlab {

    void Actor::Initialize(ComPtr<ID3D11Device> &device,
        ComPtr<ID3D11DeviceContext>& context,
        shared_ptr<Model>& model) {
            m_model = model;
        D3D11Utils::CreateConstBuffer(device, m_actorConstsCPU,
                                          m_actorConstsGPU);
            m_actorConstsCPU.world = Matrix();

        m_actorConstsCPU.indexColor = IndexColorMaker::getInstance().GetNewIndexColor();
    }


    void Actor::UpdateConstantBuffers(
             ComPtr<ID3D11Device> &device,
             ComPtr<ID3D11DeviceContext> &context) {
        //TODO Model에 경우엔 모아서 한번만 하는게..ㅎ
        m_model->UpdateConstantBuffers(device, context);
        D3D11Utils::UpdateBuffer(device, context, m_actorConstsCPU,
                                     m_actorConstsGPU);      
    }

    void Actor::Render(ComPtr<ID3D11DeviceContext> &context) { 
        if (m_isVisible) {
            context->VSSetConstantBuffers(10, 1,
                                          m_actorConstsGPU.GetAddressOf());
            context->PSSetConstantBuffers(10, 1,
                                          m_actorConstsGPU.GetAddressOf());
            m_model->Render(context);
        } 
    }
    
    void Actor::UpdateWorldRow(const Matrix &worldRow) {
        m_boundingSphere.Center = worldRow.Translation();
        m_worldMatrix = worldRow;
        Matrix worldITRow = worldRow;
        worldITRow.Translation(Vector3(0.0f));
        worldITRow = worldRow.Invert().Transpose();

        m_actorConstsCPU.world = worldRow.Transpose();
        m_actorConstsCPU.worldIT = worldITRow.Transpose();

      
    }

    void Actor::RenderNormals(ComPtr<ID3D11DeviceContext> &context) {
        m_model->RenderNormals(context);
    }
}