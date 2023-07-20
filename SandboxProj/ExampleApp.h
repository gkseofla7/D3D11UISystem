#pragma once

#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>

#include "AppBase.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Model.h"
#include "DragDropButton.h"
#include "Actor.h"


namespace hlab {

using DirectX::BoundingSphere;
using DirectX::SimpleMath::Vector3;

class ExampleApp : public AppBase {
  public:
    ExampleApp();

    virtual bool Initialize() override;
    bool InitializeModel();
    virtual void UpdateGUI() override;
    virtual void Update(float dt) override;
    virtual void Render() override;

    void UpdateLights(float dt);
    void PickIndexColorFromRT();
  protected:
    shared_ptr<Model> m_ground;
    shared_ptr<Model> m_mainObj;
    
    shared_ptr<Model> m_skybox;
    shared_ptr<Model> m_cursorSphereModel;
    shared_ptr<Model> m_screenSquare;
    shared_ptr<Model> m_uiSquare;
    shared_ptr<Model> m_uiButton;
    shared_ptr<Model> m_lightSphereModel;
    
    shared_ptr<Model> m_sphere;
    shared_ptr<Model> m_square;

    shared_ptr<Actor> m_lightSphere[MAX_LIGHTS];
    shared_ptr<Actor> m_mainActor;
    shared_ptr<Actor> m_groundActor;
    shared_ptr<Actor> m_mirrorActor;
    shared_ptr<Actor> m_cursorSphere;

    shared_ptr<Actor> m_selectedActor;
    
    vector<shared_ptr<DragDropButton>> m_uiButtons;
    shared_ptr<DragDropButton> m_dragdropButton;

    BoundingSphere m_mainBoundingSphere;
    DirectX::BoundingBox m_groundBoundingBox;

    bool m_usePerspectiveProjection = true;

    // 거울
    shared_ptr<Model> m_mirror;
    DirectX::SimpleMath::Plane m_mirrorPlane;
    float m_mirrorAlpha = 1.0f; // Opacity

    // 거울이 아닌 물체들의 리스트 (for문으로 그리기 위함)
    vector<shared_ptr<Actor>> m_basicList;

    float m_uiMaxX = 0.f;
    float m_uiMinX = 0.f;
    float m_uiMaxY = 0.f;
    float m_uiMinY = 0.f;
    int m_selectButtonIndex = -1;

    uint16_t m_pickColor[4] = {} ;
};

} // namespace hlab
