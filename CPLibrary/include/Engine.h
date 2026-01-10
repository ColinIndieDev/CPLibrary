#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <queue>
#include <random>
#include <vector>

#include "Colors.h"
#include "KeyInputs.h"
#include "shape3D/Frustum.h"
#include "util/Logging.h"
#include <GLFW/glfw3.h>

namespace CPL {
enum class DrawModes : uint8_t {
    SHAPE_2D,
    TEX,
    TEXT,
    SHAPE_2D_LIGHT,
    TEX_LIGHT,
    SHAPE_3D,
    CUBE_TEX,
    SHAPE_3D_LIGHT,
    CUBE_TEX_LIGHT,
};
enum class TextureFiltering : uint8_t {
    NEAREST,
    LINEAR,
};
enum class PostProcessingModes : uint8_t {
    DEFAULT,
    INVERSE,
    GRAYSCALE,
    BLUR,
    SHARP,
    EDGE_DETECTION,
};

struct Color {
    Color() : r(0), g(0), b(0), a(0) {}
    Color(const float r, const float g, const float b, const float a)
        : r(r), g(g), b(b), a(a) {}
    Color(const float val) : r(val), g(val), b(val), a(val) {}
    float r, g, b, a;
};

struct Timer;
class TimerManager;

class Shader;
class GlobalLight;
class PointLight;
class Triangle;
class Rectangle;
class Circle;
class Line;
class Texture2D;
class ParticleSystem;

class Cube;
class Sphere;
class CubeTex;
class PlaneTex;
class PointLight3D;
class DirectionalLight;
class CubeMap;
class ShadowMap;

struct Plane;
class Frustum;

struct Camera2D {
    glm::vec2 position{0.0f};
    float zoom = 1.0f;
    float rotation = 0.0f;

    [[nodiscard]] glm::mat4 GetViewMatrix() const {
        auto view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(-position, 0.0f));

        view = glm::translate(view, glm::vec3(position, 0.0f));
        view = glm::rotate(view, glm::radians(rotation), glm::vec3(0, 0, 1));
        view = glm::translate(view, glm::vec3(-position, 0.0f));

        view = glm::translate(view, glm::vec3(position, 0.0f));
        view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));
        view = glm::translate(view, glm::vec3(-position, 0.0f));

        return view;
    }
    [[nodiscard]] static glm::mat4
    GetProjectionMatrix(const glm::ivec2 &screenSize) {
        return glm::ortho(0.0f, static_cast<float>(screenSize.x),
                          static_cast<float>(screenSize.y), 0.0f, -1.0f, 1.0f);
    }
};

struct Camera3D {
    glm::vec3 position{0};
    glm::vec3 front{0, 0, -1};
    glm::vec3 up{0, 1, 0};
    bool firstMouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 800.0f / 2.0;
    float lastY = 600.0 / 2.0;
    float fov = 45.0f;
    float sensitivity = 0.1f;

    Frustum frustum;

    [[nodiscard]] glm::mat4 GetViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }
    [[nodiscard]] glm::mat4 GetProjectionMatrix(const float aspect) const {
        return glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
    }

    void UpdateFrustum(const float aspect) {
        frustum.Update(GetProjectionMatrix(aspect) * GetViewMatrix());
    }
};
class ScreenQuad;

struct Character;
class Text;

struct Audio;
class AudioManager;
} // namespace CPL

class Engine {
  public:
    static void UpdateCPL();

    static bool CheckCollisionRects(const CPL::Rectangle &one,
                                    const CPL::Rectangle &two);
    static bool CheckCollisionCircleRect(const CPL::Circle &one,
                                         const CPL::Rectangle &two);
    static bool CheckCollisionVec2Rect(const glm::vec2 &one,
                                       const CPL::Rectangle &two);
    static bool CheckCollisionCircles(const CPL::Circle &one,
                                      const CPL::Circle &two);
    static bool CheckCollisionVec2Circle(const glm::vec2 &one,
                                         const CPL::Circle &two);

    static void InitShaders();
    static CPL::DrawModes &GetCurMode();
    static CPL::Shader &GetShader(const CPL::DrawModes &mode);

    static void DrawTriangle(const glm::vec2 &pos, const glm::vec2 &size,
                             const CPL::Color &color);
    static void DrawTriangleRot(const glm::vec2 &pos, const glm::vec2 &size,
                                float angle, const CPL::Color &color);
    static void DrawTriangleOut(const glm::vec2 &pos, const glm::vec2 &size,
                                const CPL::Color &color);
    static void DrawTriangleRotOut(const glm::vec2 &pos, const glm::vec2 &size,
                                   float angle, const CPL::Color &color);
    static void DrawRect(const glm::vec2 &pos, const glm::vec2 &size,
                         const CPL::Color &color);
    static void DrawRectRot(const glm::vec2 &pos, const glm::vec2 &size,
                            float angle, const CPL::Color &color);
    static void DrawRectOut(const glm::vec2 &pos, const glm::vec2 &size,
                            const CPL::Color &color);
    static void DrawRectRotOut(const glm::vec2 &pos, const glm::vec2 &size,
                               float angle, const CPL::Color &color);
    static void DrawCircle(const glm::vec2 &pos, float radius,
                           const CPL::Color &color);
    static void DrawCircleOut(const glm::vec2 &pos, float radius,
                              const CPL::Color &color);
    static void DrawLine(const glm::vec2 &startPos, const glm::vec2 &endPos,
                         const CPL::Color &color);
    static void DrawTex2D(CPL::Texture2D *tex, const glm::vec2 &pos,
                          const CPL::Color &color);
    static void DrawTex2DRot(CPL::Texture2D *tex, const glm::vec2 &pos,
                             float angle, const CPL::Color &color);

    static void DrawText(const glm::vec2 &pos, float scale,
                         const std::string &text, const CPL::Color &color);
    static void DrawTextShadow(const glm::vec2 &pos, const glm::vec2 &shadowOff,
                               float scale, const std::string &text,
                               const CPL::Color &color,
                               const CPL::Color &shadowColor);

    static void DrawCube(const glm::vec3 &pos, const glm::vec3 &size,
                         const CPL::Color &color);
    static void DrawSphere(const glm::vec3 &pos, float radius,
                           const CPL::Color &color);
    static void DrawCubeTex(const CPL::Texture2D *tex, const glm::vec3 &pos,
                            const glm::vec3 &size, const CPL::Color &color);
    static void DrawCubeTexAtlas(const CPL::Texture2D *tex,
                                 const glm::vec3 &pos, const glm::vec3 &size,
                                 const CPL::Color &color);
    static void DrawPlaneTex(const CPL::Texture2D *tex, const glm::vec3 &pos,
                             const glm::vec2 &size, const CPL::Color &color);
    static void DrawPlaneTexRot(const CPL::Texture2D *tex, const glm::vec3 &pos,
                                const glm::vec3 &rot, const glm::vec2 &size,
                                const CPL::Color &color);
    static void DrawCubeMap(const CPL::CubeMap *map);
    static void DrawCubeMapRot(CPL::CubeMap *map, const glm::vec3 &rot);

    static void ResetShader();

    static void ClearBackground(const CPL::Color &color);
    static void BeginDraw(const CPL::DrawModes &mode, bool mode2D);
    static void SetAmbientLight2D(float strength);
    static void SetGlobalLight2D(const CPL::GlobalLight &light);
    static void AddPointLights2D(const std::vector<CPL::PointLight> &lights);
    static void AddPointLights(float lights);
    static void SetShininess3D(float shininess);
    static void AddPointLights3D(const std::vector<CPL::PointLight3D> &lights);
    static void SetDirLight3D(const CPL::DirectionalLight &light);
    static void BeginPostProcessing();
    static void EndPostProcessing();
    static void ApplyPostProcessing(const CPL::PostProcessingModes &mode);
    static void ApplyPostProcessingCustom(const CPL::Shader &shader);
    static void EndDraw();

    static void FramebufferSizeCallback(GLFWwindow *window, int width,
                                        int height);
    static void MouseCallback(GLFWwindow *window, double xPosIn, double yPosIn);
    static void CharCallback(GLFWwindow *window, uint32_t codepoint);
    static void InitCharPressed(GLFWwindow *window);

    static void CalcFPS();
    static int GetFPS();
    static void CalcDeltaTime();
    static float GetDeltaTime();
    static float GetTime();
    static void SetTimeScale(float scale);

    static void ShowDetails();

    static std::pair<int, int> GetOpenGLVersion(std::string version);
    static void InitWindow(int width, int height, const char *title,
                           bool openGLDebug, const std::string &openGLVersion);
    static void SetWindowIcon(const std::string &filePath);
    static void DestroyWindow();
    static void CloseWindow();

    static void EnableVSync(bool enabled);
    static void EnableFaceCulling(bool enabled);
    static void LockMouse(bool enabled);
    static bool WindowShouldClose();
    static float GetScreenWidth();
    static float GetScreenHeight();

    static int RandInt(int min, int max);
    static float RandFloat(float min, float max);
    static bool RandPercentInt(int percent);
    static bool RandPercentFloat(float percent);

    static void UpdateInput();
    static bool IsKeyDown(int key);
    static bool IsKeyUp(int key);
    static bool IsKeyPressedOnce(int key);
    static bool IsKeyReleased(int key);
    static uint32_t GetCharPressed();
    static bool IsMouseDown(int button);
    static bool IsMousePressedOnce(int button);
    static bool IsMouseReleased(int button);

    static glm::vec2 GetMousePos();
    static glm::vec2 GetScreenToWorld2D(const glm::vec2 &screenPos);

    static CPL::Camera2D &GetCam2D();
    static CPL::Camera3D &GetCam3D();

    static GLFWwindow *GetWindow();
    static CPL::Shader &GetScreenQuadShader();
    static CPL::Shader &GetCubeMapShader();
    static CPL::Shader &GetDepthShader();

  private:
    static uint32_t s_ScreenWidth;
    static uint32_t s_ScreenHeight;
    static glm::mat4 s_Projection2D;
    static glm::mat4 s_Projection3D;

    static CPL::Shader s_Shape2DShader;
    static CPL::Shader s_TextShader;
    static CPL::Shader s_TextureShader;
    static CPL::Shader s_LightShape2DShader;
    static CPL::Shader s_LightTextureShader;
    static CPL::Shader s_ScreenShader;

    static CPL::Shader s_Shape3DShader;
    static CPL::Shader s_CubeTexShader;
    static CPL::Shader s_LightShape3DShader;
    static CPL::Shader s_LightCubeTexShader;
    static CPL::Shader s_CubeMapShader;
    static CPL::Shader s_DepthShader;

    static CPL::DrawModes s_CurrentDrawMode;

    static std::mt19937 s_Gen;

    static std::unordered_map<int, bool> s_KeyStates;
    static std::unordered_map<int, bool> s_PrevKeyStates;
    static std::unordered_map<int, bool> s_MouseButtons;
    static std::unordered_map<int, bool> s_PrevMouseButtons;

    static GLFWwindow *s_Window;
    static std::queue<uint32_t> s_CharQueue;

    static CPL::Camera2D s_Camera2D;
    static CPL::Camera3D s_Camera3D;
    static CPL::ScreenQuad s_ScreenQuad;

    static bool s_CharInputEnabled;

    static double s_LastTime;
    static int s_NBFrames;
    static int s_FPS;
    static float s_DeltaTime;
    static float s_LastFrame;
    static float s_TimeScale;
};
