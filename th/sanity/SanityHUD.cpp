#include "plugin.h"
#include "mm/imgui/imgui.h"

#include "mm/core/avasingle.h"
#include "mm/core/graphics/graphicsengine.h"
#include "mm/core/input.h"

#include "mm/mm.h"
#include "mm/game/go/go.h"
#include "mm/game/game.h"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <mm/imgui/imgui/imgui_impl_dx11.h>
#include <mm/game/go/character.h>
#include <mm/game/go/vehicle.h>
#include <mm/game/charactermanager.h>
#include <mm/imgui/imgui/imgui_internal.h>
#include <unordered_map>


std::unordered_map<std::string, ImTextureID> g_TextureCache;


// --------- Loader helper (DirectX11) ----------
ImTextureID LoadTextureFromFileDX11(const char* filename, ID3D11Device* device, ID3D11DeviceContext* context) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4); // RGBA
    if (!data) {
        Log("[SanityHUD] Failed to load texture: %s\n", filename);
        return nullptr;
    }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA subResource = {};
    subResource.pSysMem = data;
    subResource.SysMemPitch = width * 4;

    ID3D11Texture2D* pTexture = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, &subResource, &pTexture);
    if (FAILED(hr)) {
        Log("[SanityHUD] Failed CreateTexture2D for %s\n", filename);
        stbi_image_free(data);
        return nullptr;
    }

    ID3D11ShaderResourceView* textureView = nullptr;
    hr = device->CreateShaderResourceView(pTexture, NULL, &textureView);
    pTexture->Release();
    stbi_image_free(data);

    if (FAILED(hr)) {
        Log("[SanityHUD] Failed CreateShaderResourceView for %s\n", filename);
        return nullptr;
    }

    return (ImTextureID)textureView;
}

// --------- HUD ----------
class SanityHUD : public ImGuiRenderer {
public:

    float MAX_SANITY = 1000.0f;
    float sanity = 1000.0;
    ImTextureID calmTexture = nullptr;
    ImTextureID madTextures[5] = { nullptr };
    ImTextureID frameTextures[12] = { nullptr };    

    bool texturesLoaded = false;
    bool force_hide = true;
    int lastTouchedFrame = -1;
    
    ImTextureID flashbackTexture = nullptr;
    bool flashbackActive = false;
    float flashStart = 0.0f;

    float sanityPercent() {
        return sanity / MAX_SANITY;
    }

    SanityHUD() : ImGuiRenderer() { }


    void Touch() {
        lastTouchedFrame = ImGui::GetFrameCount();
        SetForceHide(false);
    }

    void SetForceHide(bool hide) {
        SendRenderEvent([this, hide]() { force_hide = hide; });
    }

    void Game() override {
        /*
        if (CGameState::m_InMainMenu || CGameState::m_State != CGameState::E_GAME_RUN || IsGuiOccludingMainDraw()) {
            SendRenderEvent([this]() { force_hide = true; });
            return;
        }
        
        SendRenderEvent([this]() { force_hide = false; });
        */
    }

    void GameHandleEvent(Event const& _event) override {
        if (_event.hash == HASHSTR("focus")) {
            CAvaSingleInstance_EXE(CDeviceManager, ->GetInputManager()->SetInFocus(!_event.Arg<bool>(0)));
            CAvaSingle<NGraphicsEngine::CGraphicsEngine>::Instance->SetCursor(_event.Arg<bool>(0) ? 0 : -1);
        }
    }

    void Render() override {

        if (force_hide) return;

        int currentFrame = ImGui::GetFrameCount();
        if (currentFrame > lastTouchedFrame + 1) { // allowing 1 frame lag
            force_hide = true;
        }


        CCharacter* player = CAvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
        if (!player) return;

        CVehicle* vehicle = player->GetVehiclePtr();

        if (!texturesLoaded) {
            LoadTextures();
            texturesLoaded = true;
        }

        // ---- madness overlay ----
        float sanityRatio = sanity / MAX_SANITY;
        float madness = 1.0f - sanityRatio;

        ImTextureID tex = nullptr;

        if (vehicle) {
            tex = calmTexture; // textura CALM.png cargada en LoadTextures()
        }
        else {
            int level = (MAX_SANITY - sanity) / 200.0f;
            if (level < 0) level = 0;
            if (level > 4) level = 4;
            tex = madTextures[level];
        }

        if (!tex) return;

        float t = ImGui::GetTime();

        // ---- frecuencia y amplitud del pulso ----
        // curva cuadrática → en bajos madness ≈ 0, en altos pega fuerte
        float pulse = 0.05f * (madness * madness); // hasta +5% a madness = 1
        float speed = 2.0f + 4.0f * madness;       // frecuencia también depende del nivel

        // escala "palpitante"
        float scale = 1.0f + pulse * sinf(t * speed * 2.0f * IM_PI);

        // posición base (esquina superior izquierda del HUD)
        ImVec2 basePos = ImVec2(30, 30);

        // relación 470x270
        float baseHeight = 80.0f;
        float baseWidth = baseHeight * (470.0f / 270.0f);

        // centro de la imagen
        ImVec2 center = ImVec2(basePos.x + baseWidth * 0.5f, basePos.y + baseHeight * 0.5f);

        // tamaño escalado
        ImVec2 size = ImVec2(baseWidth * scale, baseHeight * scale);

        // recalcular coordenadas a partir del centro
        ImVec2 topLeft = ImVec2(center.x - size.x * 0.5f, center.y - size.y * 0.5f);
        ImVec2 bottomRight = ImVec2(center.x + size.x * 0.5f, center.y + size.y * 0.5f);

        // ---- efecto de color pulsante sobre la imagen ----
        float r = 1.0f, g = 1.0f, b = 1.0f;
        if (!vehicle && sanity <= 10) { // Only on critical sanity and outside vehicle
            float colorPulse = 0.3f * madness * sinf(t * 3.0f * 2.0f * IM_PI);
            g = 1.0f - colorPulse;
            b = 1.0f - colorPulse;
        }
        ImVec4 pulseColor(r, g, b, 1.0f);

        // dibujar en drawlist
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();
        draw_list->AddImage(
            tex,
            topLeft,
            bottomRight,
            ImVec2(0, 0), ImVec2(1, 1),
            IM_COL32(
                (int)(pulseColor.x * 255),
                (int)(pulseColor.y * 255),
                (int)(pulseColor.z * 255),
                (int)(pulseColor.w * 255)
            )
        );

        // ---- barra de progreso arriba de la imagen ----
        float barHeight = 5.0f;
        float barMargin = 5.0f; // margen desde el borde superior
        float barWidth = size.x * sanityRatio;

        ImVec2 barPos = ImVec2(basePos.x, basePos.y + barMargin);
        ImVec4 barColor = GetSanityColor(sanityRatio);

        draw_list->AddRectFilled(
            barPos,
            ImVec2(barPos.x + barWidth, barPos.y + barHeight),
            IM_COL32(
                (int)(barColor.x * 255),
                (int)(barColor.y * 255),
                (int)(barColor.z * 255),
                (int)(barColor.w * 255)
            )
        );

        // opcional: borde de la barra
        draw_list->AddRect(
            barPos,
            ImVec2(basePos.x + size.x, barPos.y + barHeight),
            IM_COL32(255, 255, 255, 100)
        );
        
        RenderFlashback(draw_list);


        // Siempre mostrar, pero con intensidad que crece bajo el 50%
        float intensity = 0.0f;
        if (sanity < MAX_SANITY * 0.5f) {
            float ratio = sanity / (MAX_SANITY * 0.5f);
            intensity = 1.0f - ratio;
        }

        // Cambio de frame animado
        static float frameTimer = 0.0f;
        static int currentIdx = 0;
        const float frameInterval = 0.02f;

        frameTimer += ImGui::GetIO().DeltaTime; // tiempo desde último frame
        size_t frameCount = sizeof(frameTextures) / sizeof(frameTextures[0]);
        if (frameTimer >= frameInterval) {
            int newIdx;
            do {
                newIdx = rand() % frameCount;
            } while (newIdx == currentIdx && frameCount > 1);

            currentIdx = newIdx;
            frameTimer = 0.0f;
        }

        ImTextureID frameTex = frameTextures[currentIdx];

        // ---- Opacidad con límite del 30% ----
        const int maxAlpha = (int)(0.5f * 255.0f);

        int baseAlpha = (int)(intensity * maxAlpha * 0.8f); // 80% del máximo como base
        float now = ImGui::GetTime();
        float framePulse = (sinf(now * (2.0f + 6.0f * intensity)) + 1.0f) * 0.5f;
        int pulseAlpha = (int)(framePulse * intensity * (maxAlpha * 0.2f));

        int finalAlpha = baseAlpha + pulseAlpha;
        if (finalAlpha > maxAlpha) finalAlpha = maxAlpha;

        ImVec2 screenMin = ImGui::GetMainViewport()->Pos;
        ImVec2 screenMax = ImVec2(
            screenMin.x + ImGui::GetMainViewport()->Size.x,
            screenMin.y + ImGui::GetMainViewport()->Size.y
        );

        ImU32 tint = IM_COL32(255, 255, 255, finalAlpha);
        draw_list->AddImage(frameTex, screenMin, screenMax,
            ImVec2(0, 0), ImVec2(1, 1), tint);

    }

    void fireFlashback(const std::string& filename) {
        if (flashbackActive) return;
        flashbackTexture = LoadTexture(filename);
        flashbackActive = true;
        flashStart = ImGui::GetTime();
    }

    void RenderFlashback(ImDrawList* draw_list) {
        if (!flashbackActive || !flashbackTexture) return;

        float now = ImGui::GetTime();
        float elapsed = now - flashStart;

        float fadeInTime = 0.3f;
        float holdTime = 0.2f;
        float fadeOutTime = 0.5f;
        float totalTime = fadeInTime + holdTime + fadeOutTime;

        float alphaNorm = 0.0f;

        if (elapsed < fadeInTime) {
            alphaNorm = elapsed / fadeInTime;
        }
        else if (elapsed < fadeInTime + holdTime) {
            alphaNorm = 1.0f;
        }
        else if (elapsed < totalTime) {
            float outT = (elapsed - fadeInTime - holdTime) / fadeOutTime;
            alphaNorm = 1.0f - outT;
        }
        else {
            flashbackActive = false;
            return;
        }

        ImVec2 screenMin = ImGui::GetMainViewport()->Pos;
        ImVec2 screenMax = ImVec2(
            screenMin.x + ImGui::GetMainViewport()->Size.x,
            screenMin.y + ImGui::GetMainViewport()->Size.y
        );

        int alpha = (int)(alphaNorm * 255.0f);
        ImU32 tint = IM_COL32(255, 255, 255, alpha);

        float sanityRatio = sanity / MAX_SANITY;

        // ---- Probabilidad de distorsión ----
        float distortionProb = 0.3f + (1.0f - sanityRatio) * 0.6f;
        bool applyDistortion = ((rand() % 1000) / 1000.0f) < distortionProb;

        // ---- Margen de oversize para cubrir shake ----
        float oversize = 50.0f; // px extra alrededor
        ImVec2 minPos = ImVec2(screenMin.x - oversize, screenMin.y - oversize);
        ImVec2 maxPos = ImVec2(screenMax.x + oversize, screenMax.y + oversize);

        if (applyDistortion) {
            float maxShake = 30.0f + (1.0f - sanityRatio) * 40.0f;
            float shakeStrength = maxShake * (1.0f - (elapsed / totalTime));

            float dx = ((rand() % 2001) / 1000.0f - 1.0f) * shakeStrength;
            float dy = ((rand() % 2001) / 1000.0f - 1.0f) * shakeStrength;

            minPos.x += dx;
            minPos.y += dy;
            maxPos.x += dx;
            maxPos.y += dy;
        }

        draw_list->AddImage(
            flashbackTexture,
            minPos,
            maxPos,
            ImVec2(0, 0), ImVec2(1, 1),
            tint
        );
    }



private:

    void LoadTextures() {
        void* backendUserData = ImGui::GetIO().BackendRendererUserData;
        if (!backendUserData) 
            return;  

        // Cast seguro dentro del cpp (sólo para uso interno, evita el header)
        struct ImGui_ImplDX11_Data { ID3D11Device* pd3dDevice; ID3D11DeviceContext* pd3dDeviceContext; };
        ImGui_ImplDX11_Data* dx11_data = (ImGui_ImplDX11_Data*)backendUserData;

        ID3D11Device* device = dx11_data->pd3dDevice;
        ID3D11DeviceContext* context = dx11_data->pd3dDeviceContext;

        madTextures[0] = LoadTextureFromFileDX11("scripts/th/textures/mad/MAD1.png", device, context);
        madTextures[1] = LoadTextureFromFileDX11("scripts/th/textures/mad/MAD2.png", device, context);
        madTextures[2] = LoadTextureFromFileDX11("scripts/th/textures/mad/MAD3.png", device, context);
        madTextures[3] = LoadTextureFromFileDX11("scripts/th/textures/mad/MAD4.png", device, context);
        madTextures[4] = LoadTextureFromFileDX11("scripts/th/textures/mad/MAD5.png", device, context);
        calmTexture = LoadTextureFromFileDX11("scripts/th/textures/mad/CALM.png", device, context);

        frameTextures[0] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_1.png", device, context);
        frameTextures[1] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_2.png", device, context);
        frameTextures[2] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_3.png", device, context);
        frameTextures[3] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_4.png", device, context);
        frameTextures[4] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_5.png", device, context);
        frameTextures[5] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_6.png", device, context);
        frameTextures[6] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_7.png", device, context);
        frameTextures[7] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_8.png", device, context);
        frameTextures[8] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_9.png", device, context);
        frameTextures[9] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_10.png", device, context);
        frameTextures[10] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_11.png", device, context);
        frameTextures[11] = LoadTextureFromFileDX11("scripts/th/textures/mad/frames/madness_frame_12.png", device, context);
    }


    ImTextureID LoadTexture(const std::string& filename) {
        auto it = g_TextureCache.find(filename);
        if (it != g_TextureCache.end()) {
            return it->second;
        }

        void* backendUserData = ImGui::GetIO().BackendRendererUserData;
        if (!backendUserData) return nullptr;

        struct ImGui_ImplDX11_Data {
            ID3D11Device* pd3dDevice;
            ID3D11DeviceContext* pd3dDeviceContext;
        };

        ImGui_ImplDX11_Data* dx11_data = (ImGui_ImplDX11_Data*)backendUserData;

        ID3D11Device* device = dx11_data->pd3dDevice;
        ID3D11DeviceContext* context = dx11_data->pd3dDeviceContext;

        ImTextureID tex = LoadTextureFromFileDX11(filename.c_str(), device, context);

        if (tex) {
            g_TextureCache[filename] = tex;
        }
        else {
            Log("[SanityHUD] Error loading texture: %s\n", filename.c_str());
        }
        return tex;
    }



    ImVec4 GetSanityColor(float ratio) {
        if (ratio > 0.66f) {
            return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        }
        else {
            float t = (ratio - 0.33f) / 0.33f;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            float g = t * 1.0f;
            return ImVec4(1.0f, g, 0.0f, 1.0f);
        }
    }


};


// global
SanityHUD sanityHud;
