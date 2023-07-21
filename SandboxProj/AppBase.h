#pragma once

#include <directxtk/SimpleMath.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <vector>
#include <directxtk/SimpleMath.h>

#include "Camera.h"
#include "ConstantBuffers.h"
#include "D3D11Utils.h"
#include "GraphicsPSO.h"
#include "PostProcess.h"
#include "Actor.h"

namespace hlab {
using namespace DirectX;
using DirectX::BoundingSphere;
using DirectX::SimpleMath::Quaternion;
using DirectX::SimpleMath::Vector3;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;
using std::shared_ptr;
using std::vector;
using std::wstring;

enum DragType
{
    NONE,
    GROUNDDRAG,
    UPDOWNLEFTRIGHTDRAG,
    ROTATEDRAG
};

class AppBase {
  public:
    AppBase();
    virtual ~AppBase();

    int Run();
    float GetAspectRatio() const;

    virtual bool Initialize();
    virtual void UpdateGUI() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void OnMouseMove(int mouseX, int mouseY);
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void InitCubemaps(wstring basePath, wstring envFilename,
                      wstring specularFilename, wstring irradianceFilename,
                      wstring brdfFilename);
    void UpdateGlobalConstants(const Vector3 &eyeWorld, const Matrix &viewRow,
                               const Matrix &projRow, const Matrix &refl);
    void SetGlobalConsts(ComPtr<ID3D11Buffer> &globalConstsGPU);


    void CreateDepthBuffers();
    void SetPipelineState(const GraphicsPSO &pso);
    //Rotate에 경우엔 무조건 BoundingSphere
    bool UpdateMouseControlRotate(const BoundingSphere &bs, Quaternion &q,
                             Vector3 &pickPoint);

  protected: // 상속 받은 클래스에서도 접근 가능
    bool InitMainWindow();
    bool InitDirect3D();
    bool InitGUI();
    void CreateBuffers();
    void SetMainViewport();
    void SetShadowViewport();

  public:
    // 변수 이름 붙이는 규칙은 VS DX11/12 기본 템플릿을 따릅니다.
    // 변수 이름을 줄이기 위해 d3d는 생략했습니다.
    // 예: m_d3dDevice -> m_device
    int m_screenWidth; // 렌더링할 최종 화면의 해상도
    int m_screenHeight;
    HWND m_mainWindow;
    bool m_useMSAA = true;
    UINT m_numQualityLevels = 0;
    bool m_drawAsWire = false;

    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_backBufferRTV;

    // 삼각형 레스터화 -> float(MSAA) -> resolved(No MSAA)
    // -> 후처리(블룸, 톤매핑) -> backBuffer(최종 SwapChain Present)
    ComPtr<ID3D11Texture2D> m_floatBuffer;
    ComPtr<ID3D11Texture2D> m_resolvedBuffer;
    ComPtr<ID3D11Texture2D> m_postEffectsBuffer;
    ComPtr<ID3D11RenderTargetView> m_floatRTV;
    ComPtr<ID3D11RenderTargetView> m_resolvedRTV;
    ComPtr<ID3D11RenderTargetView> m_postEffectsRTV;
    ComPtr<ID3D11ShaderResourceView> m_resolvedSRV;
    ComPtr<ID3D11ShaderResourceView> m_postEffectsSRV;

    ComPtr<ID3D11Texture2D> m_indexBuffer;
    ComPtr<ID3D11Texture2D> m_resolvedIndexBuffer;
    ComPtr<ID3D11RenderTargetView> m_indexRTV;
    ComPtr<ID3D11RenderTargetView> m_resolveIndexRTV;
    ComPtr<ID3D11Texture2D> m_indexStagingBuffer; // 1x1 작은 크기


    // Depth buffer 관련
    ComPtr<ID3D11Texture2D> m_depthOnlyBuffer; // No MSAA
    ComPtr<ID3D11DepthStencilView> m_depthOnlyDSV;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    ComPtr<ID3D11ShaderResourceView> m_depthOnlySRV;

    // Shadow maps
    int m_shadowWidth = 1280;
    int m_shadowHeight = 1280;
    ComPtr<ID3D11Texture2D> m_shadowBuffers[MAX_LIGHTS]; // No MSAA
    ComPtr<ID3D11DepthStencilView> m_shadowDSVs[MAX_LIGHTS];
    ComPtr<ID3D11ShaderResourceView> m_shadowSRVs[MAX_LIGHTS];

    D3D11_VIEWPORT m_screenViewport;

    // 시점을 결정하는 카메라 클래스 추가
    Camera m_camera;
    bool m_keyPressed[256] = {
        false,
    };

    bool m_leftButton = false;
    bool m_rightButton = false;
    DragType m_dragType = DragType::NONE;

    
    float m_cursorNdcX = 0.0f;
    float m_cursorNdcY = 0.0f;
    // 마우스 커서 위치 저장 (Picking에 사용)
    int m_cursorX = 0;
    int m_cursorY = 0;
    // 렌더링 -> PostEffects -> PostProcess
    PostEffectsConstants m_postEffectsConstsCPU;
    ComPtr<ID3D11Buffer> m_postEffectsConstsGPU;

    PostProcess m_postProcess;

    // 다양한 Pass들을 더 간단히 구현하기 위해 ConstBuffer들 분리
    GlobalConstants m_globalConstsCPU;
    GlobalConstants m_reflectGlobalConstsCPU;
    GlobalConstants m_shadowGlobalConstsCPU[MAX_LIGHTS];
    ComPtr<ID3D11Buffer> m_globalConstsGPU;
    ComPtr<ID3D11Buffer> m_reflectGlobalConstsGPU;
    ComPtr<ID3D11Buffer> m_shadowGlobalConstsGPU[MAX_LIGHTS];

    // 공통으로 사용하는 텍스춰들
    ComPtr<ID3D11ShaderResourceView> m_envSRV;
    ComPtr<ID3D11ShaderResourceView> m_irradianceSRV;
    ComPtr<ID3D11ShaderResourceView> m_specularSRV;
    ComPtr<ID3D11ShaderResourceView> m_brdfSRV;

    bool m_lightRotate = false;

    public:
    bool UpdateMouseControlTranslate(const shared_ptr<Actor> actor,
                                     const DirectX::BoundingBox &bsq,
                                     Vector3 &dragTranslation,
                                     Vector3 &pickPoint);
    template <typename T>
    bool UpdateMouseControlTranslate(const T &bs,
                                              const DirectX::BoundingBox &bsq,
                                              Vector3 &dragTranslation,
                                              Vector3 &pickPoint) {

        const Matrix viewRow = m_camera.GetViewRow();
        const Matrix projRow = m_camera.GetProjRow();

        // mainSphere의 회전 계산용
        static float prevRatio = 0.0f;
        static Vector3 prevPos(0.0f);
        static Vector3 prevVector(0.0f);

        // 이동 초기화
        dragTranslation = Vector3(0.0f);

        if (m_leftButton) {
            // 마우스가 Ground에 있을때, 없을때!
            Vector3 cursorNdcNear = Vector3(m_cursorNdcX, m_cursorNdcY, 0.0f);
            Vector3 cursorNdcFar = Vector3(m_cursorNdcX, m_cursorNdcY, 1.0f);

            // NDC 커서 위치를 월드 좌표계로 역변환 해주는 행렬
            Matrix inverseProjView = (viewRow * projRow).Invert();

            // ViewFrustum 안에서 PickingRay의 방향 구하기
            Vector3 cursorWorldNear =
                Vector3::Transform(cursorNdcNear, inverseProjView);
            Vector3 cursorWorldFar =
                Vector3::Transform(cursorNdcFar, inverseProjView);
            Vector3 dir = cursorWorldFar - cursorWorldNear;
            dir.Normalize();
            // 광선을 만들고 충돌 감지
            SimpleMath::Ray curRay = SimpleMath::Ray(cursorWorldNear, dir);

            float dist = 0.0f;
            float groundDist = 0.0f;
            if (curRay.Intersects(bs, dist)) { // 내 물체가 Ray에 맞을경우
                pickPoint = cursorWorldNear + dist * dir;
                if (curRay.Intersects(bsq, groundDist)) {
                    auto pickGroundPoint = cursorWorldNear + groundDist * dir;
                    // 원이다 보니 바닥의 Normal Vector 방향으로 위로 이동
                    // pickPoint += Vector3(0.0f, 1.0f, 0.0f) *( bs.Radius
                    // / 2.0f+0.2f);
                    //
                    if (m_dragType !=
                        DragType::GROUNDDRAG) { // 드래그를 시작하는 경우
                        m_dragType = DragType::GROUNDDRAG;
                        prevPos = pickPoint;
                    } else {
                        Vector3 newPos = pickGroundPoint;
                        if ((newPos - prevPos).Length() > 1e-3) {
                            dragTranslation = newPos - prevPos;
                            prevPos = newPos;
                        }
                    }
                } else {
                    if (m_dragType !=
                        DragType::UPDOWNLEFTRIGHTDRAG) { // 드래그를 시작하는
                                                         // 경우
                        m_dragType = DragType::UPDOWNLEFTRIGHTDRAG;
                        prevRatio =
                            dist / (cursorWorldFar - cursorWorldNear).Length();
                        prevPos = pickPoint;
                    } else {
                        Vector3 newPos =
                            cursorWorldNear +
                            prevRatio * (cursorWorldFar - cursorWorldNear);
                        if ((newPos - prevPos).Length() > 1e-3) {
                            dragTranslation = newPos - prevPos;
                            prevPos = newPos;
                        }
                    }
                }

                return true; // selected
            }
        }

        return false;
    }
};
} // namespace hlab