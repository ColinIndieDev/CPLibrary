#include "../include/CPL.h"
#include "../include/Engine.h"
#include <GLFW/glfw3.h>

namespace CPL {
void UpdateCPL() { Engine::UpdateCPL(); }

void ShowDetails() { Engine::ShowDetails(); }

bool CheckCollisionRects(const Rectangle &one, const Rectangle &two) {
    return Engine::CheckCollisionRects(one, two);
}
bool CheckCollisionCircleRect(const Circle &one, const Rectangle &two) {
    return Engine::CheckCollisionCircleRect(one, two);
}
bool CheckCollisionVec2Rect(const glm::vec2 &one, const Rectangle &two) {
    return Engine::CheckCollisionVec2Rect(one, two);
}
bool CheckCollisionCircles(const Circle &one, const Circle &two) {
    return Engine::CheckCollisionCircles(one, two);
}
bool CheckCollisionVec2Circle(const glm::vec2 &one, const Circle &two) {
    return Engine::CheckCollisionVec2Circle(one, two);
}

void InitWindow(const glm::ivec2 &size, const std::string &title, const bool openGLDebug, const std::string &openGLVersion) {
    Engine::InitWindow(size.x, size.y, title.c_str(), openGLDebug, openGLVersion);
}
void InitWindow(const glm::ivec2 &size, const std::string &title, const std::string &openGLVersion) {
    Engine::InitWindow(size.x, size.y, title.c_str(), false, openGLVersion);
}
void SetWindowIcon(const std::string &filePath) {
    Engine::SetWindowIcon(filePath);
}
void DestroyWindow() { Engine::DestroyWindow(); }
void CloseWindow() { Engine::CloseWindow(); }

void BeginDraw(const DrawModes &mode, const bool mode2D) {
    Engine::BeginDraw(mode, mode2D);
}
void SetAmbientLight2D(const float strength) {
    Engine::SetAmbientLight2D(strength);
}
void SetGlobalLight2D(const GlobalLight &light) {
    Engine::SetGlobalLight2D(light);
}
void AddPointLights2D(const std::vector<PointLight> &lights) {
    Engine::AddPointLights2D(lights);
}
void SetShininess3D(const float shininess) {
    Engine::SetShininess3D(shininess);
}
void AddPointLights3D(const std::vector<PointLight3D> &lights) {
    Engine::AddPointLights3D(lights);
}
void SetDirLight3D(const DirectionalLight &light) {
    Engine::SetDirLight3D(light);
}
void BeginPostProcessing() { Engine::BeginPostProcessing(); }
void EndPostProcessing() { Engine::EndPostProcessing(); }
void ApplyPostProcessing(const PostProcessingModes &mode) {
    Engine::ApplyPostProcessing(mode);
}
void ApplyPostProcessingCustom(const Shader &shader) {
    Engine::ApplyPostProcessingCustom(shader);
}
void DrawTriangle(const glm::vec2 &position, const glm::vec2 &size,
                  const Color &color) {
    Engine::DrawTriangle(position, size, color);
}
void DrawTriangleRot(const glm::vec2 &pos, const glm::vec2 &size,
                     const float angle, const Color &color) {
    Engine::DrawTriangleRot(pos, size, angle, color);
}
void DrawTriangleOut(const glm::vec2 &pos, const glm::vec2 &size,
                     const Color &color) {
    Engine::DrawTriangleOut(pos, size, color);
}
void DrawTriangleRotOut(const glm::vec2 &pos, const glm::vec2 &size,
                        const float angle, const Color &color) {
    Engine::DrawTriangleRotOut(pos, size, angle, color);
}
void DrawRect(const glm::vec2 &pos, const glm::vec2 &size, const Color &color) {
    Engine::DrawRect(pos, size, color);
}
void DrawRectRot(const glm::vec2 &pos, const glm::vec2 &size, const float angle,
                 const Color &color) {
    Engine::DrawRectRot(pos, size, angle, color);
}
void DrawRectOut(const glm::vec2 &pos, const glm::vec2 &size,
                 const Color &color) {
    Engine::DrawRectOut(pos, size, color);
}
void DrawRectangleRotOut(const glm::vec2 &pos, const glm::vec2 &size,
                         const float angle, const Color &color) {
    Engine::DrawRectRotOut(pos, size, angle, color);
}
void DrawCircle(const glm::vec2 &pos, const float radius, const Color &color) {
    Engine::DrawCircle(pos, radius, color);
}
void DrawCircleOut(const glm::vec2 &pos, const float radius,
                   const Color &color) {
    Engine::DrawCircleOut(pos, radius, color);
}
void DrawLine(const glm::vec2 &startPos, const glm::vec2 &endPos,
              const Color &color) {
    Engine::DrawLine(startPos, endPos, color);
}
void DrawTex2D(Texture2D *const tex, const glm::vec2 &pos, const Color &color) {
    Engine::DrawTex2D(tex, pos, color);
}
void DrawTex2DRot(Texture2D *const tex, const glm::vec2 &pos, const float angle,
                  const Color &color) {
    Engine::DrawTex2DRot(tex, pos, angle, color);
}
void DrawText(const glm::vec2 &pos, const float scale, const std::string &text,
              const Color &color) {
    Engine::DrawText(pos, scale, text, color);
}
void DrawTextShadow(const glm::vec2 &pos, const glm::vec2 &shadowOff,
                    const float scale, const std::string &text,
                    const Color &color, const Color &shadowColor) {
    Engine::DrawTextShadow(pos, shadowOff, scale, text, color, shadowColor);
}
std::string GetDefaultFont() { return "defaultFont"; }
void DrawCube(const glm::vec3 &pos, const glm::vec3 &size, const Color &color) {
    Engine::DrawCube(pos, size, color);
}
void DrawSphere(const glm::vec3 &pos, const float radius, const Color &color) {
    Engine::DrawSphere(pos, radius, color);
}
void DrawCubeTex(const Texture2D *const tex, const glm::vec3 &pos,
                 const glm::vec3 &size, const Color &color) {
    Engine::DrawCubeTex(tex, pos, size, color);
}
void DrawCubeTexAtlas(const Texture2D *const tex, const glm::vec3 &pos,
                      const glm::vec3 &size, const Color &color) {
    Engine::DrawCubeTexAtlas(tex, pos, size, color);
}
void DrawPlaneTex(const Texture2D *const tex, const glm::vec3 &pos,
                  const glm::vec2 &size, const Color &color) {
    Engine::DrawPlaneTex(tex, pos, size, color);
}
void DrawPlaneTexRot(const Texture2D *const tex, const glm::vec3 &pos,
                     const glm::vec3 &rot, const glm::vec2 &size,
                     const Color &color) {
    Engine::DrawPlaneTexRot(tex, pos, rot, size, color);
}
void DrawCubeMap(const CubeMap *const map) { Engine::DrawCubeMap(map); }

void ClearBackground(const Color &color) { Engine::ClearBackground(color); }
void EndDraw() { Engine::EndDraw(); }

int GetFPS() { return Engine::GetFPS(); }
float GetDeltaTime() { return Engine::GetDeltaTime(); }
float GetTime() { return Engine::GetTime(); }
void SetTimeScale(const float scale) { Engine::SetTimeScale(scale); }

void LockMouse(const bool enabled) { Engine::LockMouse(enabled); }
void EnableVSync(const bool enabled) { Engine::EnableVSync(enabled); }
void EnableFaceCulling(const bool enabled) {
    Engine::EnableFaceCulling(enabled);
}
bool WindowShouldClose() { return Engine::WindowShouldClose(); }

float GetScreenWidth() { return Engine::GetScreenWidth(); }
float GetScreenHeight() { return Engine::GetScreenHeight(); }

int RandInt(const int min, const int max) { return Engine::RandInt(min, max); }
float RandFloat(const float min, const float max) {
    return Engine::RandFloat(min, max);
}
bool RandPercentInt(const int percent) {
    return Engine::RandPercentInt(percent);
}
bool RandPercentFloat(const float percent) {
    return Engine::RandPercentFloat(percent);
}

bool IsKeyDown(const int key) { return Engine::IsKeyDown(key); }
bool IsKeyUp(const int key) { return Engine::IsKeyUp(key); }
bool IsKeyPressedOnce(const int key) { return Engine::IsKeyPressedOnce(key); }
bool IsKeyReleased(const int key) { return Engine::IsKeyReleased(key); }
uint32_t GetCharPressed() { return Engine::GetCharPressed(); }
bool IsMouseDown(const int button) { return Engine::IsMouseDown(button); }
bool IsMousePressedOnce(const int button) {
    return Engine::IsMousePressedOnce(button);
}
bool IsMouseReleased(const int button) {
    return Engine::IsMouseReleased(button);
}

glm::vec2 GetMousePos() { return Engine::GetMousePos(); }
glm::vec2 GetScreenToWorld2D(const glm::vec2 &screenPos) {
    return Engine::GetScreenToWorld2D(screenPos);
}

Camera2D &GetCam2D() { return Engine::GetCam2D(); }
Camera3D &GetCam3D() { return Engine::GetCam3D(); }

GLFWwindow *GetWindow() { return Engine::GetWindow(); }

Shader &GetShader(const DrawModes &mode) { return Engine::GetShader(mode); }
DrawModes &GetCurMode() { return Engine::GetCurMode(); }
} // namespace CPL
