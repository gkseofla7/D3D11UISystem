#include "ExampleApp.h"

#include <DirectXCollision.h> // 구와 광선 충돌 계산에 사용
#include <directxtk/DDSTextureLoader.h>
#include <directxtk/SimpleMath.h>
#include <tuple>
#include <vector>
#include "DragDropButton.h"
#include "GeometryGenerator.h"
#include "GraphicsCommon.h"

namespace hlab {

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

ExampleApp::ExampleApp() : AppBase() {}

bool ExampleApp::Initialize() {

    if (!AppBase::Initialize())
        return false;

    AppBase::InitCubemaps(L"../Assets/Textures/Cubemaps/HDRI/",
                          L"OutdoorEnvHDR.dds", L"OutdoorSpecularHDR.dds",
                          L"OutdoorDiffuseHDR.dds", L"OutdoorBrdf.dds");

    // 후처리용 화면 사각형
    {
        MeshData meshData = GeometryGenerator::MakeSquare();
        m_screenSquare =
            make_shared<Model>(m_device, m_context, vector{meshData});
    }
    
    //UI 화면 사각형
    { 
        MeshData meshData = GeometryGenerator::MakeSquare();
        meshData.vertices[0].position = Vector3(-0.5f, -0.5f, 0.0f);
        meshData.vertices[1].position = Vector3(0.5f, -0.5f, 0.0f);
        meshData.vertices[2].position = Vector3(0.5f, -1.0f, 0.0f);
        meshData.vertices[3].position = Vector3(-0.5f, -1.0f, 0.0f);
        meshData.albedoTextureFilename =
             "../Assets/Textures/UITexture.jpg";

        
        for (int i = 0; i< 4 ;i++) {
            if (i ==0) {
                m_uiMaxX = meshData.vertices[i].position.x;
                m_uiMinX = meshData.vertices[i].position.x;
                m_uiMaxY = meshData.vertices[i].position.y;
                m_uiMinY = meshData.vertices[i].position.y;
                continue;
            }
            m_uiMaxX = max(m_uiMaxX, meshData.vertices[i].position.x);
            m_uiMinX = min(m_uiMinX, meshData.vertices[i].position.x);
            m_uiMaxY = max(m_uiMaxY, meshData.vertices[i].position.y);
            m_uiMinY = min(m_uiMaxY, meshData.vertices[i].position.y);
        
        }

        m_uiSquare =
            make_shared<Model>(m_device, m_context, vector{meshData});
    }
    //UI 버튼
    {
        Vector3 startPos = Vector3(m_uiMinX, (m_uiMinY+m_uiMaxY)/2.f, 0.0f);
        const float offset = 0.1f;
        const float squareLength = 0.1f;
        const int squareNum = 4;
        
        {
            float uiX = m_uiMaxX - m_uiMinX;
            float uiY = m_uiMaxY - m_uiMinY;
            float xSquareDist = (uiX - 2.0f * offset) / (squareNum);
            Vector3 Pos = Vector3(0.f, 0.f, 0.f);
            MeshData meshData = GeometryGenerator::MakeSquare();
            meshData.vertices[0].position = Vector3(
                Pos.x - squareLength / 2.f, Pos.y - squareLength / 2.f, 0.f);
            meshData.vertices[1].position = Vector3(
                Pos.x - squareLength / 2.f, Pos.y + squareLength / 2.f, 0.f);
            meshData.vertices[2].position = Vector3(
                Pos.x + squareLength / 2.f, Pos.y + squareLength / 2.f, 0.f);
            meshData.vertices[3].position = Vector3(
                Pos.x + squareLength / 2.f, Pos.y - squareLength / 2.f, 0.f);
            meshData
                .albedoTextureFilename = /*"../Assets/Textures/UITexture.jpg";*/
                "../Assets/Textures/PBR/older-wood-flooring-ue/"
                "older-wood-flooring_roughness.png";
            m_uiButton =
                make_shared<Model>(m_device, m_context, vector{meshData});

            for (int i = 0; i < squareNum; i++) {
                shared_ptr<DragDropButton> uiButton =
                    make_shared<DragDropButton>(m_device, m_context,
                                                m_uiButton);
                m_uiButtons.push_back(uiButton);
            }

            startPos.x = startPos.x + offset + xSquareDist / 2.f;
            for (int i = 0; i < squareNum; i++) {
                Vector3 Pos = startPos;
                Pos.x += +xSquareDist * i;
                m_uiButtons[i]->m_buttonConstsCPU.buttonLength = squareLength;
                m_uiButtons[i]->m_buttonConstsCPU.screenPos =
                    Vector2(Pos.x, Pos.y);
                m_uiButtons[i]->UpdateConstantBuffers(m_device, m_context);
            }
        }


        {
        //Todo
            //vector<MeshData> meshData = {
            //    GeometryGenerator::MakeSphere(0.4f, 50, 50)};
            //m_square = make_shared<Model>(m_device, m_context, meshData);
            //m_square->m_materialConstsCPU.invertNormalMapY =
            //    true; // GLTF는 true로
            //m_square->m_materialConstsCPU.albedoFactor = Vector3(1.0f);
            //m_square->m_materialConstsCPU.roughnessFactor = 0.3f;
            //m_square->m_materialConstsCPU.metallicFactor = 0.8f;
            //// 아싸리 Model을
            //for (int i = 0; i < squareNum; i++) {

            //    shared_ptr<Actor> actor =
            //        make_shared<Actor>(m_device, m_context, m_square);
            //    m_dynamicActors.push_back(actor);
            //}
        }
        {
            Vector3 Pos = Vector3(0.f, 0.f, 0.f);
            m_dragdropButton =
                make_shared<DragDropButton>(m_device, m_context, m_uiButton);
            m_dragdropButton->m_buttonConstsCPU.screenPos =
                Vector2(Pos.x, Pos.y);
            m_dragdropButton->m_buttonConstsCPU.buttonLength = squareLength;
            m_dragdropButton->UpdateConstantBuffers(m_device, m_context);
        }


    }
    // 환경 박스 초기화
    {
        MeshData skyboxMesh = GeometryGenerator::MakeBox(40.0f);
        std::reverse(skyboxMesh.indices.begin(), skyboxMesh.indices.end());
        m_skybox = make_shared<Model>(m_device, m_context, vector{skyboxMesh});
    }
    

    // Model 정의
    {
        {
            // auto meshes = GeometryGenerator::ReadFromFile(
            //     "../Assets/Models/DamagedHelmet/", "DamagedHelmet.gltf");

            // auto meshes = GeometryGenerator::ReadFromFile(
            //     "../Assets/Models/medieval_vagrant_knights/", "scene.gltf",
            //     true);

            // 컴퓨터가 느릴 때는 간단한 물체로 테스트 하세요.
            vector<MeshData> meshes = {GeometryGenerator::MakeSphere(0.4f, 50, 50)};

            // string path = "../Assets/Characters/armored-female-future-soldier/";
            // auto meshes = GeometryGenerator::ReadFromFile(path,
            // "angel_armor.fbx"); meshes[0].albedoTextureFilename = path +
            // "/angel_armor_albedo.jpg"; meshes[0].emissiveTextureFilename = path +
            // "/angel_armor_e.jpg"; meshes[0].metallicTextureFilename = path +
            // "/angel_armor_metalness.jpg"; meshes[0].normalTextureFilename = path
            // + "/angel_armor_normal.jpg"; meshes[0].roughnessTextureFilename =
            //     path + "/angel_armor_roughness.jpg";

            m_mainObj = make_shared<Model>(m_device, m_context, meshes);
            m_mainObj->m_materialConstsCPU.invertNormalMapY = true; // GLTF는 true로
            m_mainObj->m_materialConstsCPU.albedoFactor = Vector3(1.0f);
            m_mainObj->m_materialConstsCPU.roughnessFactor = 0.3f;
            m_mainObj->m_materialConstsCPU.metallicFactor = 0.8f;
        }
        //바닥
        {
            auto mesh = GeometryGenerator::MakeSquare(5.0);
            // mesh.albedoTextureFilename =
            //     "../Assets/Textures/blender_uv_grid_2k.png";
            m_ground = make_shared<Model>(m_device, m_context, vector{mesh});
            m_ground->m_materialConstsCPU.albedoFactor = Vector3(0.1f);
            m_ground->m_materialConstsCPU.emissionFactor = Vector3(0.0f);
            m_ground->m_materialConstsCPU.metallicFactor = 0.5f;
            m_ground->m_materialConstsCPU.roughnessFactor = 0.3f;
            m_mirror = m_ground;
        }
        // 추가 물체 1
        {
            MeshData mesh = GeometryGenerator::MakeSphere(0.2f, 200, 200);

            m_sphere = make_shared<Model>(m_device, m_context, vector{mesh});
            m_sphere->m_materialConstsCPU.albedoFactor = Vector3(0.1f, 0.1f, 1.0f);
            m_sphere->m_materialConstsCPU.roughnessFactor = 0.2f;
            m_sphere->m_materialConstsCPU.metallicFactor = 0.6f;
            m_sphere->m_materialConstsCPU.emissionFactor = Vector3(0.0f);
        }
            // 추가 물체2
        {
            MeshData mesh = GeometryGenerator::MakeBox(0.2f);
            
            m_square = make_shared<Model>(m_device, m_context, vector{mesh});
            m_square->m_materialConstsCPU.albedoFactor = Vector3(1.0f, 0.2f, 0.2f);
            m_square->m_materialConstsCPU.roughnessFactor = 0.5f;
            m_square->m_materialConstsCPU.metallicFactor = 0.9f;
            m_square->m_materialConstsCPU.emissionFactor = Vector3(0.0f);
        }
        //Light Model
        {
            MeshData sphere = GeometryGenerator::MakeSphere(1.0f, 20, 20);
            m_lightSphereModel =
                make_shared<Model>(m_device, m_context, vector{sphere});
            m_lightSphereModel->m_materialConstsCPU.albedoFactor =
                Vector3(0.0f);
            m_lightSphereModel->m_materialConstsCPU.emissionFactor =
                Vector3(1.0f, 1.0f, 0.0f);
        }

            // 커서 표시 (Main sphere와의 충돌이 감지되면 월드 공간에 작게 그려지는
        // 구)
        {
            MeshData sphere = GeometryGenerator::MakeSphere(0.01f, 10, 10);
            m_cursorSphereModel =
                make_shared<Model>(m_device, m_context, vector{sphere});
            m_cursorSphereModel->m_isVisible =
                false; // 마우스가 눌렸을 때만 보임
            m_cursorSphereModel->m_materialConstsCPU.albedoFactor =
                Vector3(0.0f);
            m_cursorSphereModel->m_materialConstsCPU.emissionFactor =
                Vector3(0.0f, 1.0f, 0.0f);
        }

    }
    //Object 정의
    {
        {// Main Object
            m_mainActor = make_shared<Actor>(m_device, m_context, m_mainObj);
            Vector3 center(0.0f, 0.0f, 2.0f);
            m_mainActor->UpdateWorldRow(Matrix::CreateTranslation(center));

            m_basicList.push_back(m_mainActor); // 리스트에 등록

            // 동일한 크기와 위치에 BoundingSphere 만들기
            m_mainBoundingSphere = BoundingSphere(center, 0.4f);
        }
            // 바닥(거울)
        {

            m_groundActor = make_shared<Actor>(m_device, m_context, m_ground);
            Vector3 position = Vector3(0.0f, -0.5f, 2.0f);
            m_groundActor->UpdateWorldRow(
                Matrix::CreateRotationX(3.141592f * 0.5f) *
                Matrix::CreateTranslation(position));

            m_mirrorPlane =
                SimpleMath::Plane(position, Vector3(0.0f, 1.0f, 0.0f));
            m_mirrorActor = m_groundActor; // 바닥에 거울처럼 반사 구현

            // m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
        }
        {
            auto obj = make_shared<Actor>(m_device, m_context, m_sphere);
            Vector3 center(0.5f, 0.5f, 2.0f);
            obj->UpdateWorldRow(Matrix::CreateTranslation(center));
            obj->UpdateConstantBuffers(m_device, m_context);
            m_basicList.push_back(obj);
        }
        {
            auto obj = make_shared<Actor>(m_device, m_context, m_square);
            Vector3 center(0.0f, 0.5f, 2.5f);
            obj->UpdateWorldRow(Matrix::CreateTranslation(center));
            obj->UpdateConstantBuffers(m_device, m_context);
            m_basicList.push_back(obj);
        }
        
    }
        

    // 조명 설정
    {
        // 조명 0은 고정
        m_globalConstsCPU.lights[0].radiance = Vector3(5.0f);
        m_globalConstsCPU.lights[0].position = Vector3(0.0f, 1.5f, 1.1f);
        m_globalConstsCPU.lights[0].direction = Vector3(0.0f, -1.0f, 0.0f);
        m_globalConstsCPU.lights[0].spotPower = 3.0f;
        m_globalConstsCPU.lights[0].radius = 0.02f;
        m_globalConstsCPU.lights[0].type =
            LIGHT_SPOT | LIGHT_SHADOW; // Point with shadow

        // 조명 1의 위치와 방향은 Update()에서 설정
        m_globalConstsCPU.lights[1].radiance = Vector3(5.0f);
        m_globalConstsCPU.lights[1].spotPower = 3.0f;
        m_globalConstsCPU.lights[1].fallOffEnd = 20.0f;
        m_globalConstsCPU.lights[1].radius = 0.02f;
        m_globalConstsCPU.lights[1].type =
            LIGHT_SPOT | LIGHT_SHADOW; // Point with shadow

        // 조명 2는 꺼놓음
        m_globalConstsCPU.lights[2].type = LIGHT_OFF;
    }

    // 조명 위치 표시
    {
        for (int i = 0; i < MAX_LIGHTS; i++) {
            
            m_lightSphere[i] =
            make_shared<Actor>(m_device, m_context, m_lightSphereModel);
            m_lightSphere[i]->UpdateWorldRow(Matrix::CreateTranslation(
                m_globalConstsCPU.lights[i].position));

            if (m_globalConstsCPU.lights[i].type == 0)
                m_lightSphere[i]->m_isVisible = false;
            m_lightSphere[i]->m_castShadow =
                false; // 조명 표시 물체들은 그림자 X
            m_basicList.push_back(m_lightSphere[i]); // 리스트에 등록
        }
    }

    // 커서 표시 (Main sphere와의 충돌이 감지되면 월드 공간에 작게 그려지는 구)
    {
        m_cursorSphere =
            make_shared<Actor>(m_device, m_context, m_cursorSphereModel);
        m_cursorSphere->m_castShadow = false;  // 그림자 X
        m_basicList.push_back(m_cursorSphere); // 리스트에 등록
    }

    return true;
}

void ExampleApp::UpdateLights(float dt) {

    // 회전하는 lights[1] 업데이트
    static Vector3 lightDev = Vector3(1.0f, 0.0f, 0.0f);
    if (m_lightRotate) {
        lightDev = Vector3::Transform(
            lightDev, Matrix::CreateRotationY(dt * 3.141592f * 0.5f));
    }
    m_globalConstsCPU.lights[1].position = Vector3(0.0f, 1.1f, 2.0f) + lightDev;
    Vector3 focusPosition = Vector3(0.0f, -0.5f, 1.7f);
    m_globalConstsCPU.lights[1].direction =
        focusPosition - m_globalConstsCPU.lights[1].position;
    m_globalConstsCPU.lights[1].direction.Normalize();

    // 그림자맵을 만들기 위한 시점
    for (int i = 0; i < MAX_LIGHTS; i++) {
        const auto &light = m_globalConstsCPU.lights[i];
        if (light.type & LIGHT_SHADOW) {

            Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
            if (abs(up.Dot(light.direction) + 1.0f) < 1e-5)
                up = Vector3(1.0f, 0.0f, 0.0f);

            // 그림자맵을 만들 때 필요
            Matrix lightViewRow = XMMatrixLookAtLH(
                light.position, light.position + light.direction, up);

            Matrix lightProjRow = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(120.0f), 1.0f, 0.1f, 10.0f);

            m_shadowGlobalConstsCPU[i].eyeWorld = light.position;
            m_shadowGlobalConstsCPU[i].view = lightViewRow.Transpose();
            m_shadowGlobalConstsCPU[i].proj = lightProjRow.Transpose();
            m_shadowGlobalConstsCPU[i].invProj =
                lightProjRow.Invert().Transpose();
            m_shadowGlobalConstsCPU[i].viewProj =
                (lightViewRow * lightProjRow).Transpose();

            // LIGHT_FRUSTUM_WIDTH 확인
            // Vector4 eye(0.0f, 0.0f, 0.0f, 1.0f);
            // Vector4 xLeft(-1.0f, -1.0f, 0.0f, 1.0f);
            // Vector4 xRight(1.0f, 1.0f, 0.0f, 1.0f);
            // eye = Vector4::Transform(eye, lightProjRow);
            // xLeft = Vector4::Transform(xLeft, lightProjRow.Invert());
            // xRight = Vector4::Transform(xRight, lightProjRow.Invert());
            // xLeft /= xLeft.w;
            // xRight /= xRight.w;
            // cout << "LIGHT_FRUSTUM_WIDTH = " << xRight.x - xLeft.x << endl;

            D3D11Utils::UpdateBuffer(m_device, m_context,
                                     m_shadowGlobalConstsCPU[i],
                                     m_shadowGlobalConstsGPU[i]);

            // 그림자를 실제로 렌더링할 때 필요
            m_globalConstsCPU.lights[i].viewProj =
                m_shadowGlobalConstsCPU[i].viewProj;
            m_globalConstsCPU.lights[i].invProj =
                m_shadowGlobalConstsCPU[i].invProj;

            // 반사된 장면에서도 그림자를 그리고 싶다면 조명도 반사시켜서
            // 넣어주면 됩니다.
        }
    }
}

void ExampleApp::Update(float dt) {

    // 카메라의 이동
    m_camera.UpdateKeyboard(dt, m_keyPressed);

    // 반사 행렬 추가
    const Vector3 eyeWorld = m_camera.GetEyePos();
    const Matrix reflectRow = Matrix::CreateReflection(m_mirrorPlane);
    const Matrix viewRow = m_camera.GetViewRow();
    const Matrix projRow = m_camera.GetProjRow();

    UpdateLights(dt);

    // 공용 ConstantBuffer 업데이트
    AppBase::UpdateGlobalConstants(eyeWorld, viewRow, projRow, reflectRow);

    // 거울은 따로 처리
    m_mirror->UpdateConstantBuffers(m_device, m_context);

    // 조명의 위치 반영
    for (int i = 0; i < MAX_LIGHTS; i++)
        m_lightSphere[i]->UpdateWorldRow(
            Matrix::CreateScale(
                std::max(0.01f, m_globalConstsCPU.lights[i].radius)) *
            Matrix::CreateTranslation(m_globalConstsCPU.lights[i].position));

    // 마우스 이동/회전 반영
    if (m_leftButton || m_rightButton) {
        Quaternion q;
        Vector3 dragTranslation;
        Vector3 pickPoint;
        if (UpdateMouseControl(m_mainBoundingSphere, q, dragTranslation,
                               pickPoint)) {
            Vector3 translation = m_mainObj->m_worldRow.Translation();
            m_mainObj->m_worldRow.Translation(Vector3(0.0f));
            m_mainActor->UpdateWorldRow(
                m_mainObj->m_worldRow * Matrix::CreateFromQuaternion(q) *
                Matrix::CreateTranslation(dragTranslation + translation));
            m_mainBoundingSphere.Center = m_mainObj->m_worldRow.Translation();

            // 충돌 지점에 작은 구 그리기
            m_cursorSphere->m_isVisible = true;
            m_cursorSphere->UpdateWorldRow(
                Matrix::CreateTranslation(pickPoint));
        } else {
            m_cursorSphere->m_isVisible = false;
        }
    } else {
        m_cursorSphere->m_isVisible = false;
    }

    if (m_leftButton) {
       //UI 내에서만 DragDropButton 보이게
            if (m_cursorNdcX <= m_uiMaxX && m_cursorNdcX >= m_uiMinX &&
                m_cursorNdcY <= m_uiMaxY && m_cursorNdcY >= m_uiMinY) {
                //Button이 아직 선택 안돼있으면
                 if (m_selectIndex == -1) 
                 {
                    for (int i = 0; i < m_uiButtons.size(); i++) {
                        if (m_uiButtons[i]->IsCursorInButton(m_cursorNdcX, m_cursorNdcY)) {
                            m_selectIndex = i;
                        }
                    }

                 }
                 
                 //선택했는데 이제 막 선택한거라면
                 if (m_selectIndex != -1 && m_dragdropButton->m_isVisible==false) {
                     //Button에 경우엔 스크린 좌표로 위치가 결정돼서 별도의 ConstBuffer 존재
                        m_dragdropButton->m_buttonConstsCPU.screenPos =
                        m_uiButtons[m_selectIndex]
                            ->m_buttonConstsCPU
                                .screenPos;
                    m_dragdropButton->m_isVisible = true;
                 } 
                 else if (m_dragdropButton->m_isVisible) {
                    m_dragdropButton->m_buttonConstsCPU.screenPos =
                        Vector2(m_cursorNdcX, m_cursorNdcY);
                 }
            } 
            else {//범위 밖에 나가면
                 if (m_dragdropButton->m_isVisible) {
                    m_dragdropButton->m_isVisible = false;
                    
                    //Todo
                    shared_ptr<Actor> actor = make_shared<Actor>(
                        m_device, m_context, m_uiButtons[m_selectIndex]->m_createModel);
                    //actor 위치 계산 물체가 있다면 물체 위에, 없다면 그냥 특정 z값에 스크린 좌표 움직임

                    m_dynamicActors.push_back(actor);
                    //m_dynamicObjs[m_selectIndex]->AddMeshConstBuffers(m_device);
                    //int meshConstsIndex =
                    //    m_dynamicObjs[m_selectIndex]->m_meshConstsCPUs.size() -
                    //    1;
                    //m_dynamicObjs[m_selectIndex]
                    //    ->m_meshConstsCPUs[meshConstsIndex].world;
                 }
                 
                 if (m_selectIndex!= -1) {
                 //아직 선택돼있는 상태

                 }
            }
    }
    else {
            m_dragdropButton->m_isVisible = false;
            m_selectIndex = -1;
    }
    m_dragdropButton->UpdateConstantBuffers(m_device, m_context);

    for (auto &i : m_basicList) {
        i->UpdateConstantBuffers(m_device, m_context);
    }
   
}

void ExampleApp::Render() {

    AppBase::SetMainViewport();

    // 모든 샘플러들을 공통으로 사용 (뒤에서 더 추가됩니다.)
    m_context->VSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                             Graphics::sampleStates.data());
    m_context->PSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                             Graphics::sampleStates.data());

    // 공용 텍스춰들: "Common.hlsli"에서 register(t10)부터 시작
    vector<ID3D11ShaderResourceView *> commonSRVs = {
        m_envSRV.Get(), m_specularSRV.Get(), m_irradianceSRV.Get(),
        m_brdfSRV.Get()};
    m_context->PSSetShaderResources(10, UINT(commonSRVs.size()),
                                    commonSRVs.data());

    const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    vector<ID3D11RenderTargetView *> rtvs = {m_floatRTV.Get(),
                                             m_indexRTV.Get()};

    // Depth Only Pass (RTS 생략 가능)
    m_context->OMSetRenderTargets(0, NULL, m_depthOnlyDSV.Get());
    m_context->ClearDepthStencilView(m_depthOnlyDSV.Get(), D3D11_CLEAR_DEPTH,
                                     1.0f, 0);
    AppBase::SetPipelineState(Graphics::depthOnlyPSO);
    AppBase::SetGlobalConsts(m_globalConstsGPU);
    //basicList 모두 Actor로 변경필요..ㅎ
    for (auto &i : m_basicList)
        i->Render(m_context);
    
    m_skybox->Render(m_context);
    m_mirror->Render(m_context);

    // 그림자맵 만들기
    AppBase::SetShadowViewport(); // 그림자맵 해상도
    AppBase::SetPipelineState(Graphics::depthOnlyPSO);
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (m_globalConstsCPU.lights[i].type & LIGHT_SHADOW) {
            // RTS 생략 가능
            m_context->OMSetRenderTargets(0, NULL, m_shadowDSVs[i].Get());
            m_context->ClearDepthStencilView(m_shadowDSVs[i].Get(),
                                             D3D11_CLEAR_DEPTH, 1.0f, 0);
            AppBase::SetGlobalConsts(m_shadowGlobalConstsGPU[i]);
            for (auto &i : m_basicList)
                if (i->m_castShadow && i->m_isVisible)
                    i->Render(m_context);
            m_skybox->Render(m_context);
            m_mirror->Render(m_context);
        }
    }

    // 다시 렌더링 해상도로 되돌리기
    AppBase::SetMainViewport();

    // 거울 1. 거울은 빼고 원래 대로 그리기
    for (size_t i = 0; i < rtvs.size(); i++) {
        m_context->ClearRenderTargetView(rtvs[i], clearColor);
    }
    m_context->OMSetRenderTargets(UINT(rtvs.size()), rtvs.data(),
                                  m_depthStencilView.Get());

    // 그림자맵들도 공용 텍스춰들 이후에 추가
    // 주의: 마지막 shadowDSV를 RenderTarget에서 해제한 후 설정
    vector<ID3D11ShaderResourceView *> shadowSRVs;
    for (int i = 0; i < MAX_LIGHTS; i++) {
        shadowSRVs.push_back(m_shadowSRVs[i].Get());
    }
    m_context->PSSetShaderResources(15, UINT(shadowSRVs.size()),
                                    shadowSRVs.data());

    m_context->ClearDepthStencilView(m_depthStencilView.Get(),
                                     D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                     1.0f, 0);
    AppBase::SetPipelineState(m_drawAsWire ? Graphics::defaultWirePSO
                                           : Graphics::defaultSolidPSO);
    AppBase::SetGlobalConsts(m_globalConstsGPU);

    for (auto &i : m_basicList) {
        i->Render(m_context);
    }

    // 거울 반사를 그릴 필요가 없으면 불투명 거울만 그리기
    if (m_mirrorAlpha == 1.0f)
        m_mirror->Render(m_context);

    AppBase::SetPipelineState(Graphics::normalsPSO);
    for (auto &i : m_basicList) {
        if (i->m_drawNormals)
            i->RenderNormals(m_context);
    }

    AppBase::SetPipelineState(m_drawAsWire ? Graphics::skyboxWirePSO
                                           : Graphics::skyboxSolidPSO);

    m_skybox->Render(m_context);

    if (m_mirrorAlpha < 1.0f) { // 거울을 그려야 하는 상황

        // 거울 2. 거울 위치만 StencilBuffer에 1로 표기
        AppBase::SetPipelineState(Graphics::stencilMaskPSO);

        m_mirror->Render(m_context);

        // 거울 3. 거울 위치에 반사된 물체들을 렌더링
        AppBase::SetPipelineState(m_drawAsWire ? Graphics::reflectWirePSO
                                               : Graphics::reflectSolidPSO);
        AppBase::SetGlobalConsts(m_reflectGlobalConstsGPU);

        m_context->ClearDepthStencilView(m_depthStencilView.Get(),
                                         D3D11_CLEAR_DEPTH, 1.0f, 0);

        for (auto &i : m_basicList) {
            i->Render(m_context);
        }

        AppBase::SetPipelineState(m_drawAsWire
                                      ? Graphics::reflectSkyboxWirePSO
                                      : Graphics::reflectSkyboxSolidPSO);
        m_skybox->Render(m_context);

        // 거울 4. 거울 자체의 재질을 "Blend"로 그림
        AppBase::SetPipelineState(m_drawAsWire ? Graphics::mirrorBlendWirePSO
                                               : Graphics::mirrorBlendSolidPSO);
        AppBase::SetGlobalConsts(m_globalConstsGPU);

        m_mirror->Render(m_context);

    } // end of if (m_mirrorAlpha < 1.0f)

    m_context->ResolveSubresource(m_resolvedBuffer.Get(), 0,
                                  m_floatBuffer.Get(), 0,
                                  DXGI_FORMAT_R16G16B16A16_FLOAT);

    // PostEffects
    AppBase::SetPipelineState(Graphics::postEffectsPSO);

    vector<ID3D11ShaderResourceView *> postEffectsSRVs = {m_resolvedSRV.Get(),
                                                          m_depthOnlySRV.Get()};

    // 그림자맵 확인용 임시
    // AppBase::SetGlobalConsts(m_shadowGlobalConstsGPU[0]);
    AppBase::SetGlobalConsts(m_globalConstsGPU );
    // vector<ID3D11ShaderResourceView *> postEffectsSRVs = {
    //  m_resolvedSRV.Get(), m_shadowSRVs[1].Get()};

    // 20번에 넣어줌
    m_context->PSSetShaderResources(20, UINT(postEffectsSRVs.size()),
                                    postEffectsSRVs.data());
    m_context->OMSetRenderTargets(1, m_postEffectsRTV.GetAddressOf(), NULL);
    // m_context->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), NULL);

    m_context->PSSetConstantBuffers(3, 1,
                                    m_postEffectsConstsGPU.GetAddressOf());
    m_screenSquare->Render(m_context);

    // 단순 이미지 처리와 블룸
    AppBase::SetPipelineState(Graphics::postProcessingPSO);
    m_postProcess.Render(m_context);

    //UI 렌더
    AppBase::SetPipelineState(Graphics::uiPSO);
    m_uiSquare->Render(m_context);

    AppBase::SetPipelineState(Graphics::uiButtonPSO);
    for (int i = 0; i < m_uiButtons.size(); i++) {
        m_uiButtons[i]->Render(m_context);
    }
    m_dragdropButton->Render(m_context);
}

void ExampleApp::UpdateGUI() {

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("General")) {
        ImGui::Checkbox("Use FPV", &m_camera.m_useFirstPersonView);
        ImGui::Checkbox("Wireframe", &m_drawAsWire);
        if (ImGui::Checkbox("MSAA ON", &m_useMSAA)) {
            CreateBuffers();
        }
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Skybox")) {
        ImGui::SliderFloat("Strength", &m_globalConstsCPU.strengthIBL, 0.0f,
                           5.0f);
        ImGui::RadioButton("Env", &m_globalConstsCPU.textureToDraw, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Specular", &m_globalConstsCPU.textureToDraw, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Irradiance", &m_globalConstsCPU.textureToDraw, 2);
        ImGui::SliderFloat("EnvLodBias", &m_globalConstsCPU.envLodBias, 0.0f,
                           10.0f);
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Post Effects")) {
        int flag = 0;
        flag += ImGui::RadioButton("Render", &m_postEffectsConstsCPU.mode, 1);
        ImGui::SameLine();
        flag += ImGui::RadioButton("Depth", &m_postEffectsConstsCPU.mode, 2);
        flag += ImGui::SliderFloat(
            "DepthScale", &m_postEffectsConstsCPU.depthScale, 0.0, 1.0);
        flag += ImGui::SliderFloat("Fog", &m_postEffectsConstsCPU.fogStrength,
                                   0.0, 10.0);

        if (flag)
            D3D11Utils::UpdateBuffer(m_device, m_context,
                                     m_postEffectsConstsCPU,
                                     m_postEffectsConstsGPU);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Post Processing")) {
        int flag = 0;
        flag += ImGui::SliderFloat(
            "Bloom Strength",
            &m_postProcess.m_combineFilter.m_constData.strength, 0.0f, 1.0f);
        flag += ImGui::SliderFloat(
            "Exposure", &m_postProcess.m_combineFilter.m_constData.option1,
            0.0f, 10.0f);
        flag += ImGui::SliderFloat(
            "Gamma", &m_postProcess.m_combineFilter.m_constData.option2, 0.1f,
            5.0f);
        // 편의상 사용자 입력이 인식되면 바로 GPU 버퍼를 업데이트
        if (flag) {
            m_postProcess.m_combineFilter.UpdateConstantBuffers(m_device,
                                                                m_context);
        }
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Mirror")) {

        ImGui::SliderFloat("Alpha", &m_mirrorAlpha, 0.0f, 1.0f);
        const float blendColor[4] = {m_mirrorAlpha, m_mirrorAlpha,
                                     m_mirrorAlpha, 1.0f};
        if (m_drawAsWire)
            Graphics::mirrorBlendWirePSO.SetBlendFactor(blendColor);
        else
            Graphics::mirrorBlendSolidPSO.SetBlendFactor(blendColor);

        ImGui::SliderFloat("Metallic",
                           &m_mirror->m_materialConstsCPU.metallicFactor, 0.0f,
                           1.0f);
        ImGui::SliderFloat("Roughness",
                           &m_mirror->m_materialConstsCPU.roughnessFactor, 0.0f,
                           1.0f);

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Light")) {
        // ImGui::SliderFloat3("Position",
        // &m_globalConstsCPU.lights[0].position.x,
        //                     -5.0f, 5.0f);
        ImGui::SliderFloat("Halo Radius",
                           &m_globalConstsCPU.lights[1].haloRadius, 0.0f, 2.0f);
        ImGui::SliderFloat("Halo Strength",
                           &m_globalConstsCPU.lights[1].haloStrength, 0.0f,
                           1.0f);
        ImGui::SliderFloat("Radius", &m_globalConstsCPU.lights[1].radius, 0.0f,
                           0.5f);
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Material")) {
        ImGui::SliderFloat("LodBias", &m_globalConstsCPU.lodBias, 0.0f, 10.0f);

        int flag = 0;

        flag += ImGui::SliderFloat(
            "Metallic", &m_mainObj->m_materialConstsCPU.metallicFactor, 0.0f,
            1.0f);
        flag += ImGui::SliderFloat(
            "Roughness", &m_mainObj->m_materialConstsCPU.roughnessFactor, 0.0f,
            1.0f);
        flag += ImGui::CheckboxFlags(
            "AlbedoTexture", &m_mainObj->m_materialConstsCPU.useAlbedoMap, 1);
        flag += ImGui::CheckboxFlags(
            "EmissiveTexture", &m_mainObj->m_materialConstsCPU.useEmissiveMap,
            1);
        flag += ImGui::CheckboxFlags(
            "Use NormalMapping", &m_mainObj->m_materialConstsCPU.useNormalMap,
            1);
        flag += ImGui::CheckboxFlags(
            "Use AO", &m_mainObj->m_materialConstsCPU.useAOMap, 1);
        flag += ImGui::CheckboxFlags(
            "Use HeightMapping", &m_mainObj->m_meshConstsCPU.useHeightMap, 1);
        flag += ImGui::SliderFloat(
            "HeightScale", &m_mainObj->m_meshConstsCPU.heightScale, 0.0f, 0.1f);
        flag += ImGui::CheckboxFlags(
            "Use MetallicMap", &m_mainObj->m_materialConstsCPU.useMetallicMap,
            1);
        flag += ImGui::CheckboxFlags(
            "Use RoughnessMap", &m_mainObj->m_materialConstsCPU.useRoughnessMap,
            1);

        if (flag) {
            m_mainObj->UpdateConstantBuffers(m_device, m_context);
        }

        ImGui::Checkbox("Draw Normals", &m_mainActor->m_drawNormals);

        ImGui::TreePop();
    }
}

} // namespace hlab
