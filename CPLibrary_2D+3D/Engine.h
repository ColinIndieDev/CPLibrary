#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <queue>
#include <random>
#include <vector>

#include "Colors.h"
#include "KeyInputs.h"
#include "Logging.h"
#include "shapes3D/Frustum.h"
#include <GLFW/glfw3.h>

namespace CPL {
enum class DrawModes {
    SHAPE,
    TEX,
    TEXT,
    SHAPE_LIGHT,
    TEX_LIGHT,
    CUBE,
    CUBE_TEX,
    CUBE_LIGHT,
    CUBE_TEX_LIGHT,
};
enum class TextureFiltering {
    NEAREST,
    LINEAR,
};
enum class PostProcessingModes {
    DEFAULT,
    INVERSE,
    GRAYSCALE,
    BLUR,
    SHARP,
    EDGE_DETECTION,
};

struct Color {
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
class CubeTex;
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

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }
    glm::mat4 GetProjectionMatrix(const float aspect) const {
        return glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
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

    static bool CheckCollisionRects(const CPL::Rectangle &one, const CPL::Rectangle &two);
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

    static void DrawTriangle(glm::vec2 pos, glm::vec2 size,
                             const CPL::Color &color);
    static void DrawTriangleRot(glm::vec2 pos, glm::vec2 size,
                                    float angle, const CPL::Color &color);
    static void DrawTriangleOut(glm::vec2 pos, glm::vec2 size,
                                    const CPL::Color &color);
    static void DrawTriangleRotOut(glm::vec2 pos, glm::vec2 size,
                                   float angle, const CPL::Color &color);
    static void DrawRect(glm::vec2 pos, glm::vec2 size,
                              const CPL::Color &color);
    static void DrawRectRot(glm::vec2 pos, glm::vec2 size,
                                     float angle, const CPL::Color &color);
    static void DrawRectOut(glm::vec2 pos, glm::vec2 size,
                                     const CPL::Color &color);
    static void DrawRectRotOut(glm::vec2 pos, glm::vec2 size,
                                    float angle, const CPL::Color &color);
    static void DrawCircle(glm::vec2 pos, float radius,
                           const CPL::Color &color);
    static void DrawCircleOut(glm::vec2 pos, float radius,
                                  const CPL::Color &color);
    static void DrawLine(glm::vec2 startPos, glm::vec2 endPos,
                         const CPL::Color &color);
    static void DrawTex2D(CPL::Texture2D *tex, glm::vec2 pos,
                              const CPL::Color &color);
    static void DrawTex2DRot(CPL::Texture2D *tex, glm::vec2 pos,
                                     float angle, const CPL::Color &color);

    static void DrawText(glm::vec2 pos, float scale,
                         const std::string &text, const CPL::Color &color);
    static void DrawTextShadow(glm::vec2 pos, glm::vec2 shadowOff,
                               float scale, const std::string &text,
                               const CPL::Color &color, const CPL::Color &shadowColor);

    static void DrawCube(glm::vec3 pos, glm::vec3 size,
                         const CPL::Color &color);
    static void DrawCubeTex(CPL::Texture2D *tex, glm::vec3 pos,
                                glm::vec3 size, const CPL::Color &color);
    static void DrawCubeMap(CPL::CubeMap *map);

    static void ResetShader();

    static void ClearBackground(const CPL::Color &color);
    static void BeginDraw(const CPL::DrawModes &mode, bool mode2D);
    static void SetAmbientLight2D(float strength);
    static void SetGlobalLight2D(const CPL::GlobalLight &light);
    static void AddPointLights2D(const std::vector<CPL::PointLight> &lights);
    static void AddPointLights(float lights);
    static void SetShininess3D(const float shininess);
    static void AddPointLights3D(const std::vector<CPL::PointLight3D> &lights);
    static void SetDirLight3D(const CPL::DirectionalLight &light);
    static void BeginPostProcessing();
    static void EndPostProcessing();
    static void ApplyPostProcessing(const CPL::PostProcessingModes &mode);
    static void ApplyPostProcessingCustom(const CPL::Shader &shader);
    static void EndDraw();

    static void FramebufferSizeCallback(GLFWwindow *window, const int width,
                                        const int height);
    static void MouseCallback(GLFWwindow *window, double xposIn, double yposIn);
    static void CharCallback(GLFWwindow *window, unsigned int codepoint);
    static void InitCharPressed(GLFWwindow *window);

    static void CalcFPS();
    static int GetFPS();
    static void CalcDeltaTime();
    static float GetDeltaTime();
    static float GetTime();
    static void SetTimeScale(const float scale);

    static void ShowDetails();

    static void InitWindow(int width, int height, const char *title);
    static void SetWindowIcon(const std::string &filePath);
    static void CloseWindow();

    static void EnableVSync(const bool enabled);
    static void EnableFaceCulling(const bool enabled);
    static int WindowShouldClose();
    static float GetScreenWidth();
    static float GetScreenHeight();

    static int RandInt(const int min, const int max);
    static float RandFloat(const float min, const float max);
    static bool RandPercentInt(const int percent);
    static bool RandPercentFloat(const float percent);

    static void UpdateInput();
    static bool IsKeyDown(const int key);
    static bool IsKeyUp(const int key);
    static bool IsKeyPressedOnce(const int key);
    static bool IsKeyReleased(const int key);
    static unsigned int GetCharPressed();
    static bool IsMouseDown(const int button);
    static bool IsMousePressedOnce(const int button);
    static bool IsMouseReleased(const int button);

    static glm::vec2 GetMousePos();
    static glm::vec2 GetScreenToWorld2D(const glm::vec2 &screenPos);

    static CPL::Camera2D &GetCam2D();
    static CPL::Camera3D &GetCam3D();

    static GLFWwindow *GetWindow();
    static CPL::Shader &GetScreenQuadShader();
    static CPL::Shader &GetCubeMapShader();
    static CPL::Shader &GetDepthShader();

  private:
    static unsigned int s_ScreenWidth;
    static unsigned int s_ScreenHeight;
    static glm::mat4 s_Projection2D;
    static glm::mat4 s_Projection3D;

    static CPL::Shader s_ShapeShader;
    static CPL::Shader s_TextShader;
    static CPL::Shader s_TextureShader;
    static CPL::Shader s_LightShapeShader;
    static CPL::Shader s_LightTextureShader;
    static CPL::Shader s_ScreenShader;

    static CPL::Shader s_CubeShader;
    static CPL::Shader s_CubeTexShader;
    static CPL::Shader s_LightCubeShader;
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
    static std::queue<unsigned int> s_CharQueue;

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
