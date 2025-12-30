#pragma once
#include "Engine.h"
#include <GLFW/glfw3.h>
#ifndef __EMSCRIPTEN__
#ifdef _WIN32
#define PRIORITIZE_GPU_BY_VENDOR                                               \
    extern "C" {                                                               \
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;      \
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;        \
    }
#else
#define PRIORITIZE_GPU_BY_VENDOR
#endif
#else
#define PRIORITIZE_GPU_BY_VENDOR
#endif

namespace CPL {
enum class DrawModes;
enum class TextureFiltering;
enum class PostProcessingModes;

struct Color;

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
class PointLight3D;
class DirectionalLight;
class CubeMap;

struct Plane;
class Frustum;

struct Camera2D;
struct Camera3D;
class ScreenQuad;

struct Character;
class Text;

struct Audio;
class AudioManager;

void UpdateCPL();

void ShowDetails();

bool CheckCollisionRects(const Rectangle &one, const Rectangle &two);
bool CheckCollisionCircleRect(const Circle &one, const Rectangle &two);
bool CheckCollisionVec2Rect(const glm::vec2 &one, const Rectangle &two);
bool CheckCollisionCircles(const Circle &one, const Circle &two);
bool CheckCollisionVec2Circle(const glm::vec2 &one, const Circle &two);

void InitWindow(const int width, const int height, const char *title);
void SetWindowIcon(const std::string &filePath);
void DestroyWindow();
void CloseWindow();

void BeginDraw(const DrawModes &mode, const bool mode2D = false);
void SetAmbientLight2D(const float strength);
void SetGlobalLight2D(const GlobalLight &light);
void AddPointLights2D(const std::vector<PointLight> &lights);
void SetShininess3D(const float shininess);
void AddPointLights3D(const std::vector<PointLight3D> &lights);
void SetDirLight3D(const DirectionalLight &light);
void BeginPostProcessing();
void EndPostProcessing();
void ApplyPostProcessing(const PostProcessingModes &mode);
void ApplyPostProcessingCustom(const Shader &shader);
void DrawTriangle(const glm::vec2 pos, const glm::vec2 size,
                  const Color &color);
void DrawTriangleRot(const glm::vec2 pos, const glm::vec2 size,
                     const float angle, const Color &color);
void DrawTriangleOut(const glm::vec2 pos, const glm::vec2 size,
                     const Color &color);
void DrawTriangleRotOut(const glm::vec2 pos, const glm::vec2 size,
                        const float angle, const Color &color);
void DrawRect(const glm::vec2 pos, const glm::vec2 size, const Color &color);
void DrawRectRot(const glm::vec2 pos, const glm::vec2 size, const float angle,
                 const Color &color);
void DrawRectOut(const glm::vec2 pos, const glm::vec2 size, const Color &color);
void DrawRectRotOut(const glm::vec2 pos, const glm::vec2 size,
                    const float angle, const Color &color);
void DrawCircle(const glm::vec2 pos, const float radius, const Color &color);
void DrawCircleOut(const glm::vec2 pos, const float radius, const Color &color);
void DrawLine(const glm::vec2 startPos, const glm::vec2 endPos,
              const Color &color);
void DrawTex2D(Texture2D *tex, const glm::vec2 pos, const Color &color);
void DrawTex2DRot(Texture2D *tex, const glm::vec2 pos, const float angle,
                  const Color &color);
void DrawText(const glm::vec2 pos, const float scale, const std::string &text,
              const Color &color);
void DrawTextShadow(const glm::vec2 pos, const glm::vec2 shadowOff,
                    const float scale, const std::string &text,
                    const Color &color, const Color &shadowColor);
std::string GetDefaultFont();
void DrawCube(const glm::vec3 pos, const glm::vec3 size, const Color &color);
void DrawSphere(const glm::vec3 pos, const float radius, const Color &color);
void DrawCubeTex(Texture2D *tex, const glm::vec3 pos, const glm::vec3 size,
                 const Color &color);
void DrawCubeTexAtlas(Texture2D *tex, const glm::vec3 pos, const glm::vec3 size,
                      const Color &color);
void DrawCubeMap(CubeMap *map);

void ClearBackground(const Color &color);
void EndDraw();

int GetFPS();
float GetDeltaTime();
float GetTime();
void SetTimeScale(const float scale);

void LockMouse(const bool enabled);
void EnableVSync(const bool enabled);
void EnableFaceCulling(const bool enabled);
int WindowShouldClose();

float GetScreenWidth();
float GetScreenHeight();

int RandInt(const int min, const int max);
float RandFloat(const float min, const float max);
bool RandPercentInt(const int percent);
bool RandPercentFloat(const float percent);

bool IsKeyDown(const int key);
bool IsKeyUp(const int key);
bool IsKeyPressedOnce(const int key);
bool IsKeyReleased(const int key);
unsigned int GetCharPressed();
bool IsMouseDown(const int button);
bool IsMousePressedOnce(const int button);
bool IsMouseReleased(const int button);

glm::vec2 GetMousePos();
glm::vec2 GetScreenToWorld2D(const glm::vec2 &screenPos);

Camera2D &GetCam2D();
Camera3D &GetCam3D();

GLFWwindow *GetWindow();

Shader &GetShader(const DrawModes &mode);
DrawModes &GetCurMode();
} // namespace CPL
