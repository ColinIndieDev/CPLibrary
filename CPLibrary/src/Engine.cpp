#include "../include/Engine.h"
#include "../include/Audio.h"
#include "../include/Shader.h"
#include "../include/Text.h"
#include "../include/shape2D/Circle.h"
#include "../include/shape2D/GlobalLight.h"
#include "../include/shape2D/Line.h"
#include "../include/shape2D/PointLight.h"
#include "../include/shape2D/Rectangle.h"
#include "../include/shape2D/ScreenQuad.h"
#include "../include/shape2D/Texture2D.h"
#include "../include/shape2D/Triangle.h"
#include "../include/shape3D/Cube.h"
#include "../include/shape3D/CubeMap.h"
#include "../include/shape3D/CubeTex.h"
#include "../include/shape3D/DirectionalLight.h"
#include "../include/shape3D/PlaneTex.h"
#include "../include/shape3D/PointLight3D.h"
#include "../include/shape3D/Sphere.h"
#include "../include/timer/TimerManager.h"
#include "../include/util/Logging.h"
#include "../include/util/OpenGLDebug.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

uint32_t Engine::s_ScreenWidth;
uint32_t Engine::s_ScreenHeight;
glm::mat4 Engine::s_Projection2D;
glm::mat4 Engine::s_Projection3D;

CPL::Shader Engine::s_Shape2DShader;
CPL::Shader Engine::s_TextShader;
CPL::Shader Engine::s_TextureShader;
CPL::Shader Engine::s_LightShape2DShader;
CPL::Shader Engine::s_LightTextureShader;
CPL::Shader Engine::s_ScreenShader;

CPL::Shader Engine::s_Shape3DShader;
CPL::Shader Engine::s_LightShape3DShader;
CPL::Shader Engine::s_CubeMapShader;
CPL::Shader Engine::s_DepthShader;

std::unique_ptr<CPL::Texture2D> Engine::s_WhiteTex;

CPL::DrawModes Engine::s_CurrentDrawMode;

std::mt19937 Engine::s_Gen{std::random_device{}()};

std::unordered_map<int, bool> Engine::s_KeyStates;
std::unordered_map<int, bool> Engine::s_PrevKeyStates;
std::unordered_map<int, bool> Engine::s_MouseButtons;
std::unordered_map<int, bool> Engine::s_PrevMouseButtons;

GLFWwindow *Engine::s_Window;
std::queue<uint32_t> Engine::s_CharQueue;

CPL::Camera2D Engine::s_Camera2D;
CPL::Camera3D Engine::s_Camera3D;
CPL::ScreenQuad Engine::s_ScreenQuad;

bool Engine::s_CharInputEnabled;

double Engine::s_LastTime;
int Engine::s_NBFrames;
int Engine::s_FPS;
float Engine::s_DeltaTime;
float Engine::s_LastFrame;
float Engine::s_TimeScale = 1.0f;

void Engine::UpdateCPL() {
    UpdateInput();
    CalcDeltaTime();
    CalcFPS();
    CPL::TimerManager::Update(GetDeltaTime());
    CPL::AudioManager::Update();
}

void Engine::ShowDetails() {
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *version = glGetString(GL_VERSION);
    const std::string rendererString(reinterpret_cast<const char *>(renderer));
    const std::string vendorString(reinterpret_cast<const char *>(vendor));
    const std::string versionString(reinterpret_cast<const char *>(version));

    BeginDraw(CPL::DrawModes::TEXT, false);
    const std::string fpsText = "FPS: " + std::to_string(GetFPS());
    DrawTextShadow({0, GetScreenHeight() - 130}, {2, 2}, 0.3, fpsText,
                   CPL::WHITE, CPL::DARK_GRAY);
    const std::string vendorText = "Vendor: " + vendorString;
    DrawTextShadow({0, GetScreenHeight() - 95}, {2, 2}, 0.3, vendorText,
                   CPL::WHITE, CPL::DARK_GRAY);
    const std::string rendererText = "GPU: " + rendererString;
    DrawTextShadow({0, GetScreenHeight() - 60}, {2, 2}, 0.3, rendererText,
                   CPL::WHITE, CPL::DARK_GRAY);
    const std::string versionText = "Version: " + versionString;
    DrawTextShadow({0, GetScreenHeight() - 25}, {2, 2}, 0.3, versionText,
                   CPL::WHITE, CPL::DARK_GRAY);
    EndDraw();
}

bool Engine::CheckCollisionRects(const CPL::Rectangle &one,
                                 const CPL::Rectangle &two) {
    const bool collisionX = one.pos.x + one.size.x >= two.pos.x &&
                            two.pos.x + two.size.x >= one.pos.x;
    const bool collisionY = one.pos.y + one.size.y >= two.pos.y &&
                            two.pos.y + two.size.y >= one.pos.y;

    return collisionX && collisionY;
}
bool Engine::CheckCollisionCircleRect(const CPL::Circle &one,
                                      const CPL::Rectangle &two) {
    const glm::vec2 circleCenter = one.pos;
    const glm::vec2 rectCenter = two.pos + two.size * 0.5f;
    const glm::vec2 halfExtents = two.size * 0.5f;
    const glm::vec2 difference = circleCenter - rectCenter;
    const glm::vec2 clamped = glm::clamp(difference, -halfExtents, halfExtents);
    const glm::vec2 closest = rectCenter + clamped;
    const glm::vec2 delta = closest - circleCenter;

    return glm::length(delta) <= one.radius;
}
bool Engine::CheckCollisionVec2Rect(const glm::vec2 &one,
                                    const CPL::Rectangle &two) {
    return two.pos.x < one.x && one.x < two.pos.x + two.size.x &&
           two.pos.y < one.y && one.y < two.pos.y + two.size.y;
}
bool Engine::CheckCollisionCircles(const CPL::Circle &one,
                                   const CPL::Circle &two) {
    const glm::vec2 dist = one.pos - two.pos;
    const float distanceSquared = (dist.x * dist.x) + (dist.y * dist.y);
    const float radiusSum = one.radius + two.radius;
    return distanceSquared <= radiusSum * radiusSum;
}
bool Engine::CheckCollisionVec2Circle(const glm::vec2 &one,
                                      const CPL::Circle &two) {
    const glm::vec2 dist = one - two.pos;
    const float distanceSquared = (dist.x * dist.x) + (dist.y * dist.y);
    return distanceSquared <= two.radius * two.radius;
}

bool Engine::CheckCollisionCubes(const CPL::Cube &one, const CPL::Cube &two) {
    const bool collisionX =
        one.pos.x + (one.size.x / 2) >= two.pos.x - (two.size.x / 2) &&
        two.pos.x + (two.size.x / 2) >= one.pos.x - (one.size.x / 2);
    const bool collisionY =
        one.pos.y + (one.size.y / 2) >= two.pos.y - (two.size.y / 2) &&
        two.pos.y + (two.size.y / 2) >= one.pos.y - (one.pos.y / 2);
    const bool collisionZ =
        one.pos.z + (one.size.z / 2) >= two.pos.z - (two.size.z / 2) &&
        two.pos.z + (two.size.z / 2) >= one.pos.z - (one.size.z / 2);

    return collisionX && collisionY && collisionZ;
}

std::pair<int, int> Engine::GetOpenGLVersion(std::string version) {
    version.erase(std::remove(version.begin(), version.end(), '.'),
                  version.end());
    bool onlyDigits =
        !version.empty() &&
        std::all_of(version.begin(), version.end(), [](const auto &c) {
            return !std::isdigit(static_cast<uint8_t>(c));
        });
    if (version.size() > 2 || onlyDigits) {
        Logging::Log(Logging::MessageStates::ERROR,
                     "Invaid version type for OpenGL!");
        return std::make_pair(3, 3);
    }

    std::pair<int, int> openGLVersion;
    for (int i = 0; i < 2; i++) {
        char c = version[i];
        int val = c - '0';

        if (i == 0)
            openGLVersion.first = val;
        else
            openGLVersion.second = val;
    }
    return openGLVersion;
}

void Engine::InitWindow(const int width, const int height, const char *title,
                        const bool openGLDebug,
                        const std::string &openGLVersion) {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);

    std::pair<int, int> version = GetOpenGLVersion(openGLVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.first);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.second);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, static_cast<int>(openGLDebug));

    Logging::Log(Logging::MessageStates::INFO,
                 "Using OpenGL version " + std::to_string(version.first) + "." +
                     std::to_string(version.second));

    s_ScreenWidth = width;
    s_ScreenHeight = height;

    s_Projection2D = CPL::Camera2D::GetProjectionMatrix(
        glm::ivec2(s_ScreenWidth, s_ScreenHeight));
    s_Projection3D = s_Camera3D.GetProjectionMatrix(
        static_cast<float>(s_ScreenWidth) / static_cast<float>(s_ScreenHeight));

    s_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (s_Window == nullptr) {
        Logging::Log(Logging::MessageStates::WARNING,
                     "Failed to create GLFW window");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(s_Window);
    glfwSetFramebufferSizeCallback(s_Window, FramebufferSizeCallback);

    if (!static_cast<bool>(gladLoadGLLoader(
            reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))) {
        Logging::Log(Logging::MessageStates::ERROR,
                     "Failed to initialize GLAD");
        exit(-1);
    }

    OpenGLDebug::EnableOpenGLDebug();

    InitShaders();
#ifdef __EMSCRIPTEN__
    CPL::Text::Init("/assets/fonts/default.ttf", "defaultFont",
                    CPL::TextureFiltering::NEAREST);
#else
    CPL::Text::Init("assets/fonts/default.ttf", "defaultFont",
                    CPL::TextureFiltering::NEAREST);
#endif
    CPL::AudioManager::Init();
    s_ScreenQuad.Init(width, height);
    InitCharPressed(s_Window);

    // ----- For the font & 2D textures ----- //
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#ifdef __EMSCRIPTEN__
    s_WhiteTex = std::make_unique<CPL::Texture2D>(
        "/assets/images/default/whiteTex.png", glm::vec2(100),
        CPL::TextureFiltering::NEAREST);
#else
    s_WhiteTex = std::make_unique<CPL::Texture2D>(
        "assets/images/default/whiteTex.png", glm::vec2(100),
        CPL::TextureFiltering::NEAREST);
#endif
}

void Engine::SetWindowIcon(const std::string &filePath) {
    int width = 0;
    int height = 0;
    int channels = 0;
    ;
    stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    int requiredComponents = 0;
    if (channels == 3)
        requiredComponents = 3;
    else if (channels == 4)
        requiredComponents = 4;
    std::array<GLFWimage, 1> images{};
    images[0].pixels =
        stbi_load(filePath.c_str(), &images[0].width, &images[0].height,
                  nullptr, requiredComponents);
    if (static_cast<bool>(images[0].pixels)) {
        glfwSetWindowIcon(s_Window, 1, images.data());
        stbi_image_free(images[0].pixels);
    } else {
        Logging::Log(Logging::MessageStates::ERROR, "Failed to load icon");
    }
}

void Engine::DestroyWindow() { glfwSetWindowShouldClose(s_Window, 1); }

void Engine::CloseWindow() {
    glfwTerminate();
    CPL::AudioManager::Close();
}

void Engine::LockMouse(const bool enabled) {
    if (enabled) {
        glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(s_Window, MouseCallback);
    } else {
        glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(s_Window, MouseCallback);
    }
}

void Engine::InitShaders() {
#ifdef __EMSCRIPTEN__
    s_Shape2DShader = CPL::Shader("/assets/shaders/web/vert/shader_web.vert",
                                  "/assets/shaders/web/frag/shader_web.frag");
    s_TextShader = CPL::Shader("/assets/shaders/web/vert/text_web.vert",
                               "/assets/shaders/web/frag/text_web.frag");
    s_TextureShader = CPL::Shader("/assets/shaders/web/vert/texture_web.vert",
                                  "/assets/shaders/web/frag/texture_web.frag");
    s_LightShape2DShader =
        CPL::Shader("/assets/shaders/web/vert/lightShader_web.vert",
                    "/assets/shaders/web/frag/lightShader_web.frag");
    s_LightTextureShader =
        CPL::Shader("assets/shaders/web/vert/lightTexture_web.vert",
                    "assets/shaders/web/frag/lightTexture_web.frag");
    s_ScreenShader = CPL::Shader("/assets/shaders/web/vert/screen_web.vert",
                                 "/assets/shaders/web/frag/screen_web.frag");
#else
    s_Shape2DShader = CPL::Shader("assets/shaders/default/vert/2D/shader.vert",
                                  "assets/shaders/default/frag/2D/shader.frag");
    s_TextShader = CPL::Shader("assets/shaders/default/vert/2D/text.vert",
                               "assets/shaders/default/frag/2D/text.frag");
    s_TextureShader =
        CPL::Shader("assets/shaders/default/vert/2D/texture.vert",
                    "assets/shaders/default/frag/2D/texture.frag");
    s_LightShape2DShader =
        CPL::Shader("assets/shaders/default/vert/2D/lightShader.vert",
                    "assets/shaders/default/frag/2D/lightShader.frag");
    s_LightTextureShader =
        CPL::Shader("assets/shaders/default/vert/2D/lightTexture.vert",
                    "assets/shaders/default/frag/2D/lightTexture.frag");
    s_ScreenShader = CPL::Shader("assets/shaders/default/vert/2D/screen.vert",
                                 "assets/shaders/default/frag/2D/screen.frag");

    s_Shape3DShader = CPL::Shader("assets/shaders/default/vert/3D/shape.vert",
                                  "assets/shaders/default/frag/3D/shape.frag");
    s_CubeMapShader =
        CPL::Shader("assets/shaders/default/vert/3D/cubeMapShader.vert",
                    "assets/shaders/default/frag/3D/cubeMapShader.frag");
    s_LightShape3DShader =
        CPL::Shader("assets/shaders/default/vert/3D/lightShape.vert",
                    "assets/shaders/default/frag/3D/lightShape.frag");
    s_DepthShader =
        CPL::Shader("assets/shaders/default/vert/3D/depthShader.vert",
                    "assets/shaders/default/frag/3D/depthShader.frag");
#endif
}

void Engine::BeginDraw(const CPL::DrawModes &mode, const bool mode2D) {
    CPL::Shader *shader = nullptr;
    s_CurrentDrawMode = mode;

    switch (mode) {
    case CPL::DrawModes::SHAPE_2D:
        shader = &s_Shape2DShader;
        break;
    case CPL::DrawModes::TEXT:
        shader = &s_TextShader;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        CPL::Text::Use("defaultFont");
        break;
    case CPL::DrawModes::TEX:
        shader = &s_TextureShader;
        break;
    case CPL::DrawModes::SHAPE_2D_LIGHT:
        shader = &s_LightShape3DShader;
        break;
    case CPL::DrawModes::TEX_LIGHT:
        shader = &s_LightTextureShader;
        break;
    case CPL::DrawModes::SHAPE_3D:
        shader = &s_Shape3DShader;
        break;
    case CPL::DrawModes::SHAPE_3D_LIGHT:
        shader = &s_LightShape3DShader;
        break;
    }
    shader->Use();

    if (mode == CPL::DrawModes::SHAPE_3D ||
        mode == CPL::DrawModes::SHAPE_3D_LIGHT) {
        float aspect = GetScreenWidth() / GetScreenHeight();
        shader->SetMatrix4fv("projection",
                             s_Camera3D.GetProjectionMatrix(aspect) *
                                 s_Camera3D.GetViewMatrix());
        glEnable(GL_DEPTH_TEST);
    } else {
        const glm::mat4 view = s_Camera2D.GetViewMatrix();
        const glm::mat4 viewProjection = s_Projection2D * view;
        shader->SetMatrix4fv("projection",
                             mode2D ? viewProjection : s_Projection2D);
        glDisable(GL_DEPTH_TEST);
    }
}
void Engine::ResetShader() {
    CPL::Shader *shader = nullptr;

    switch (s_CurrentDrawMode) {
    case CPL::DrawModes::SHAPE_2D:
        shader = &s_Shape2DShader;
        break;
    case CPL::DrawModes::TEXT:
        shader = &s_TextShader;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        CPL::Text::Use("defaultFont");
        break;
    case CPL::DrawModes::TEX:
        shader = &s_TextureShader;
        break;
    case CPL::DrawModes::SHAPE_2D_LIGHT:
        shader = &s_LightShape3DShader;
        break;
    case CPL::DrawModes::TEX_LIGHT:
        shader = &s_LightTextureShader;
        break;
    case CPL::DrawModes::SHAPE_3D:
        shader = &s_Shape3DShader;
        break;
    case CPL::DrawModes::SHAPE_3D_LIGHT:
        shader = &s_LightShape3DShader;
        break;
    }
    shader->Use();
}
void Engine::SetAmbientLight2D(const float strength) {
    s_LightShape2DShader.Use();
    s_LightShape2DShader.SetFloat("ambient", strength);

    s_LightTextureShader.Use();
    s_LightTextureShader.SetFloat("ambient", strength);

    ResetShader();
}
void Engine::SetGlobalLight2D(const CPL::GlobalLight &light) {
    s_LightShape2DShader.Use();
    s_LightShape2DShader.SetFloat("globalLight.intensity", light.intensity);
    s_LightShape2DShader.SetColor("globalLight.color", light.color);

    s_LightTextureShader.Use();
    s_LightTextureShader.SetFloat("globalLight.intensity", light.intensity);
    s_LightTextureShader.SetColor("globalLight.color", light.color);

    ResetShader();
}

void Engine::AddPointLights2D(const std::vector<CPL::PointLight> &lights) {
    s_LightShape2DShader.Use();
    s_LightShape2DShader.SetInt("numPointLights",
                                static_cast<int>(lights.size()));
    for (int i = 0; i < lights.size(); i++) {
        s_LightShape2DShader.SetVector2f(
            "pointLights[" + std::to_string(i) + "].position", lights[i].pos);
        s_LightShape2DShader.SetFloat(
            "pointLights[" + std::to_string(i) + "].radius", lights[i].radius);
        s_LightShape2DShader.SetFloat("pointLights[" + std::to_string(i) +
                                          "].intensity",
                                      lights[i].intensity);
        s_LightShape2DShader.SetColor(
            "pointLights[" + std::to_string(i) + "].color", lights[i].color);
    }

    s_LightTextureShader.Use();
    s_LightTextureShader.SetInt("numPointLights",
                                static_cast<int>(lights.size()));
    for (int i = 0; i < lights.size(); i++) {
        s_LightTextureShader.SetVector2f(
            "pointLights[" + std::to_string(i) + "].position", lights[i].pos);
        s_LightTextureShader.SetFloat(
            "pointLights[" + std::to_string(i) + "].radius", lights[i].radius);
        s_LightTextureShader.SetFloat("pointLights[" + std::to_string(i) +
                                          "].intensity",
                                      lights[i].intensity);
        s_LightTextureShader.SetColor(
            "pointLights[" + std::to_string(i) + "].color", lights[i].color);
    }

    ResetShader();
}
void Engine::SetShininess3D(const float shininess) {
    s_LightShape3DShader.Use();
    s_LightShape3DShader.SetFloat("shininess", shininess);

    ResetShader();
}
void Engine::AddPointLights3D(const std::vector<CPL::PointLight3D> &lights) {
    s_LightShape3DShader.Use();
    s_LightShape3DShader.SetInt("numPointLights",
                                static_cast<int>(lights.size()));
    for (int i = 0; i < lights.size(); i++) {
        s_LightShape3DShader.SetVector3f(
            "pointLights[" + std::to_string(i) + "].position", lights[i].pos);
        s_LightShape3DShader.SetFloat("pointLights[" + std::to_string(i) +
                                          "].intensity",
                                      lights[i].intensity);
        s_LightShape3DShader.SetFloat("pointLights[" + std::to_string(i) +
                                          "].constant",
                                      lights[i].constant);
        s_LightShape3DShader.SetFloat(
            "pointLights[" + std::to_string(i) + "].linear", lights[i].linear);
        s_LightShape3DShader.SetFloat("pointLights[" + std::to_string(i) +
                                          "].quadratic",
                                      lights[i].quadratic);
        s_LightShape3DShader.SetColor(
            "pointLights[" + std::to_string(i) + "].color", lights[i].color);
    }

    ResetShader();
}
void Engine::SetDirLight3D(const CPL::DirectionalLight &light) {
    s_LightShape3DShader.Use();
    s_LightShape3DShader.SetVector3f("viewPos", s_Camera3D.position);
    s_LightShape3DShader.SetVector3f("dirLight.direction", light.dir);
    s_LightShape3DShader.SetVector3f("dirLight.ambient",
                                     glm::vec3(light.ambient.r / 255,
                                               light.ambient.g / 255,
                                               light.ambient.b / 255));
    s_LightShape3DShader.SetVector3f("dirLight.diffuse", light.diffuse);
    s_LightShape3DShader.SetVector3f("dirLight.specular", light.specular);

    ResetShader();
}

void Engine::EnableFog(const bool enabled) {
    s_Shape3DShader.Use();
    s_Shape3DShader.SetBool("useFog", enabled);

    s_LightShape3DShader.Use();
    s_LightShape3DShader.SetBool("useFog", enabled);

    ResetShader();
}
void Engine::SetFog(const float fogStart, const float fogEnd,
                    const CPL::Color &color) {
    s_Shape3DShader.Use();
    s_Shape3DShader.SetFloat("fogStart", fogStart);
    s_Shape3DShader.SetFloat("fogEnd", fogEnd);
    s_Shape3DShader.SetColor("fogColor", color);

    s_LightShape3DShader.Use();
    s_LightShape3DShader.SetFloat("fogStart", fogStart);
    s_LightShape3DShader.SetFloat("fogEnd", fogEnd);
    s_LightShape3DShader.SetColor("fogColor", color);

    ResetShader();
}
void Engine::BeginPostProcessing() { s_ScreenQuad.BeginUseScreen(); }
void Engine::EndPostProcessing() { CPL::ScreenQuad::EndUseScreen(); }
void Engine::ApplyPostProcessing(const CPL::PostProcessingModes &mode) {
    s_ScreenQuad.Draw(static_cast<int>(mode));
}
void Engine::ApplyPostProcessingCustom(const CPL::Shader &shader) {
    s_ScreenQuad.DrawCustom(shader);
}

void Engine::DrawTriangle(const glm::vec2 &pos, const glm::vec2 &size,
                          const CPL::Color &color) {
    const auto triangle = CPL::Triangle(pos, size, color);
    triangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                      ? s_LightShape2DShader
                      : s_Shape2DShader,
                  true);
}
void Engine::DrawTriangleRot(const glm::vec2 &pos, const glm::vec2 &size,
                             const float angle, const CPL::Color &color) {
    const auto triangle = CPL::Triangle(pos, size, color);
    triangle.rotAngle = angle;
    triangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                      ? s_LightShape2DShader
                      : s_Shape2DShader,
                  true);
}
void Engine::DrawTriangleOut(const glm::vec2 &pos, const glm::vec2 &size,
                             const CPL::Color &color) {
    const auto triangle = CPL::Triangle(pos, size, color);
    triangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                      ? s_LightShape2DShader
                      : s_Shape2DShader,
                  false);
}
void Engine::DrawTriangleRotOut(const glm::vec2 &pos, const glm::vec2 &size,
                                const float angle, const CPL::Color &color) {
    const auto triangle = CPL::Triangle(pos, size, color);
    triangle.rotAngle = angle;
    triangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                      ? s_LightShape2DShader
                      : s_Shape2DShader,
                  false);
}

void Engine::DrawRect(const glm::vec2 &pos, const glm::vec2 &size,
                      const CPL::Color &color) {
    const auto rectangle = CPL::Rectangle(pos, size, color);
    rectangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                       ? s_LightShape2DShader
                       : s_Shape2DShader,
                   true);
}
void Engine::DrawRectRot(const glm::vec2 &pos, const glm::vec2 &size,
                         const float angle, const CPL::Color &color) {
    const auto rectangle = CPL::Rectangle(pos, size, color);
    rectangle.rotAngle = angle;
    rectangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                       ? s_LightShape2DShader
                       : s_Shape2DShader,
                   true);
}
void Engine::DrawRectOut(const glm::vec2 &pos, const glm::vec2 &size,
                         const CPL::Color &color) {
    const auto rectangle = CPL::Rectangle(pos, size, color);
    rectangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                       ? s_LightShape2DShader
                       : s_Shape2DShader,
                   false);
}
void Engine::DrawRectRotOut(const glm::vec2 &pos, const glm::vec2 &size,
                            const float angle, const CPL::Color &color) {
    const auto rectangle = CPL::Rectangle(pos, size, color);
    rectangle.rotAngle = angle;
    rectangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                       ? s_LightShape2DShader
                       : s_Shape2DShader,
                   false);
}

void Engine::DrawCircle(const glm::vec2 &pos, const float radius,
                        const CPL::Color &color) {
    const auto circle = CPL::Circle(pos, radius, color);
    circle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                    ? s_LightShape2DShader
                    : s_Shape2DShader);
}
void Engine::DrawCircleOut(const glm::vec2 &pos, const float radius,
                           const CPL::Color &color) {
    const auto circle = CPL::Circle(pos, radius, color);
    circle.DrawOutline(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                           ? s_LightShape2DShader
                           : s_Shape2DShader);
}

void Engine::DrawLine(const glm::vec2 &startPos, const glm::vec2 &endPos,
                      const CPL::Color &color) {
    const auto line = CPL::Line(startPos, endPos, color);
    line.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_2D_LIGHT
                  ? s_LightShape2DShader
                  : s_Shape2DShader);
}

void Engine::DrawTex2D(CPL::Texture2D *const tex, const glm::vec2 &pos,
                       const CPL::Color &color) {
    tex->pos = pos;
    tex->color = color;
    tex->Draw(s_CurrentDrawMode == CPL::DrawModes::TEX_LIGHT
                  ? s_LightTextureShader
                  : s_TextureShader);
}
void Engine::DrawTex2DRot(CPL::Texture2D *const tex, const glm::vec2 &pos,
                          const float angle, const CPL::Color &color) {
    tex->pos = pos;
    tex->color = color;
    tex->rotAngle = angle;
    tex->Draw(s_CurrentDrawMode == CPL::DrawModes::TEX_LIGHT
                  ? s_LightTextureShader
                  : s_TextureShader);
}

void Engine::DrawText(const glm::vec2 &pos, const float scale,
                      const std::string &text, const CPL::Color &color) {
    CPL::Text::DrawText(s_TextShader, text, pos, scale, color);
}
void Engine::DrawTextShadow(const glm::vec2 &pos, const glm::vec2 &shadowOff,
                            const float scale, const std::string &text,
                            const CPL::Color &color,
                            const CPL::Color &shadowColor) {
    CPL::Text::DrawText(s_TextShader, text,
                        {pos.x + shadowOff.x, pos.y - shadowOff.y}, scale,
                        shadowColor);
    CPL::Text::DrawText(s_TextShader, text, pos, scale, color);
}

void Engine::DrawCube(const glm::vec3 &pos, const glm::vec3 &size,
                      const CPL::Color &color) {
    const auto cube = CPL::Cube(pos, size, color);
    cube.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_3D_LIGHT
                  ? s_LightShape3DShader
                  : s_Shape3DShader);
}

void Engine::DrawSphere(const glm::vec3 &pos, const float radius,
                        const CPL::Color &color) {
    const auto sphere = CPL::Sphere(pos, radius, color);
    sphere.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_3D_LIGHT
                    ? s_LightShape3DShader
                    : s_Shape3DShader);
}

void Engine::DrawCubeTex(const CPL::Texture2D *const tex, const glm::vec3 &pos,
                         const glm::vec3 &size, const CPL::Color &color) {
    const auto cubeTex = CPL::CubeTex(pos, size, color);
    cubeTex.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_3D_LIGHT
                     ? s_LightShape3DShader
                     : s_Shape3DShader,
                 tex);
}

void Engine::DrawCubeTexAtlas(const CPL::Texture2D *const tex,
                              const glm::vec3 &pos, const glm::vec3 &size,
                              const CPL::Color &color) {
    const auto cubeTex = CPL::CubeTex(pos, size, color);
    cubeTex.DrawAtlas(s_CurrentDrawMode == CPL::DrawModes::SHAPE_3D_LIGHT
                          ? s_LightShape3DShader
                          : s_Shape3DShader,
                      tex);
}

void Engine::DrawPlaneTex(const CPL::Texture2D *const tex, const glm::vec3 &pos,
                          const glm::vec2 &size, const CPL::Color &color) {
    const auto planeTex = CPL::PlaneTex(pos, glm::vec3(0), size, color);
    planeTex.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_3D_LIGHT
                      ? s_LightShape3DShader
                      : s_Shape3DShader,
                  tex);
}

void Engine::DrawPlaneTexRot(const CPL::Texture2D *const tex,
                             const glm::vec3 &pos, const glm::vec3 &rot,
                             const glm::vec2 &size, const CPL::Color &color) {
    const auto planeTex = CPL::PlaneTex(pos, rot, size, color);
    planeTex.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_3D_LIGHT
                      ? s_LightShape3DShader
                      : s_Shape3DShader,
                  tex);
}

void Engine::DrawCubeMap(const CPL::CubeMap *const map) {
    glDepthMask(GL_FALSE);
    map->Draw(s_CubeMapShader);
    glDepthMask(GL_TRUE);
}

void Engine::DrawCubeMapRot(CPL::CubeMap *map, const glm::vec3 &rot) {
    glDepthMask(GL_FALSE);
    map->rot = rot;
    map->Draw(s_CubeMapShader);
    map->rot = glm::vec3(0);
    glDepthMask(GL_TRUE);
}

void Engine::ClearBackground(const CPL::Color &color) {
    glClearColor(color.r / 255, color.g / 255, color.b / 255, color.a / 255);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::EndDraw() { glUseProgram(0); }

void Engine::FramebufferSizeCallback(GLFWwindow *window, const int width,
                                     const int height) {
    glViewport(0, 0, width, height);
}

void Engine::MouseCallback(GLFWwindow *window, const double xPosIn,
                           const double yPosIn) {
    auto xPos = static_cast<float>(xPosIn);
    auto yPos = static_cast<float>(yPosIn);

    if (s_Camera3D.firstMouse) {
        s_Camera3D.lastX = xPos;
        s_Camera3D.lastY = yPos;
        s_Camera3D.firstMouse = false;
    }

    float xOff = xPos - s_Camera3D.lastX;
    float yOff = s_Camera3D.lastY - yPos;
    s_Camera3D.lastX = xPos;
    s_Camera3D.lastY = yPos;

    xOff *= s_Camera3D.sensitivity;
    yOff *= s_Camera3D.sensitivity;

    s_Camera3D.yaw += xOff;
    s_Camera3D.pitch += yOff;

    s_Camera3D.pitch = std::min(s_Camera3D.pitch, 89.0f);
    s_Camera3D.pitch = std::max(s_Camera3D.pitch, -89.0f);

    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(s_Camera3D.yaw)) *
                                 cos(glm::radians(s_Camera3D.pitch)));
    front.y = static_cast<float>(sin(glm::radians(s_Camera3D.pitch)));
    front.z = static_cast<float>(sin(glm::radians(s_Camera3D.yaw)) *
                                 cos(glm::radians(s_Camera3D.pitch)));
    s_Camera3D.front = glm::normalize(front);
}

void Engine::CharCallback(GLFWwindow *window, const uint32_t codepoint) {
    if (s_CharInputEnabled) {
        s_CharQueue.push(codepoint);
    } else if (!s_CharQueue.empty()) {
        s_CharQueue.pop();
    }
}

void Engine::InitCharPressed(GLFWwindow *window) {
    glfwSetCharCallback(window, CharCallback);
}

void Engine::CalcFPS() {
    const double currentTime = glfwGetTime();
    s_NBFrames++;
    if (currentTime - s_LastTime >= 1.0) {
        s_FPS = s_NBFrames;
        s_NBFrames = 0;
        s_LastTime += 1.0;
    }
}
int Engine::GetFPS() { return s_FPS; }

void Engine::CalcDeltaTime() {
    const auto currentFrame = static_cast<float>(glfwGetTime());
    s_DeltaTime = (currentFrame - s_LastFrame) * s_TimeScale;
    s_LastFrame = currentFrame;
}

float Engine::GetDeltaTime() { return s_DeltaTime; }
float Engine::GetTime() { return static_cast<float>(glfwGetTime()); }
void Engine::SetTimeScale(const float scale) { s_TimeScale = scale; }
void Engine::EnableVSync(const bool enabled) {
    glfwSwapInterval(static_cast<int>(enabled));
}
void Engine::EnableFaceCulling(const bool enabled) {
    if (enabled) {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
}
bool Engine::WindowShouldClose() {
    return static_cast<bool>(glfwWindowShouldClose(s_Window));
}

float Engine::GetScreenWidth() { return static_cast<float>(s_ScreenWidth); }
float Engine::GetScreenHeight() { return static_cast<float>(s_ScreenHeight); }

int Engine::RandInt(const int min, const int max) {
    std::uniform_int_distribution dist(min, max);
    return dist(s_Gen);
}
float Engine::RandFloat(const float min, const float max) {
    std::uniform_real_distribution dist(min, max);
    return dist(s_Gen);
}
bool Engine::RandPercentInt(const int percent) {
    std::uniform_int_distribution dist(1, 100);
    return dist(s_Gen) <= percent;
}
bool Engine::RandPercentFloat(const float percent) {
    std::uniform_real_distribution dist(0.0f, 100.0f);
    return dist(s_Gen) <= percent;
}

void Engine::UpdateInput() {
    s_PrevKeyStates = s_KeyStates;
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++) {
        s_KeyStates[key] = glfwGetKey(s_Window, key) == GLFW_PRESS;
    }

    s_PrevMouseButtons = s_MouseButtons;
    for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST;
         button++) {
        s_MouseButtons[button] =
            glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
    }
}

bool Engine::IsKeyDown(const int key) { return s_KeyStates[key]; }
bool Engine::IsKeyUp(const int key) { return !s_KeyStates[key]; }
bool Engine::IsKeyPressedOnce(const int key) {
    return s_KeyStates[key] && !s_PrevKeyStates[key];
}
bool Engine::IsKeyReleased(const int key) {
    return !s_KeyStates[key] && s_PrevKeyStates[key];
}
uint32_t Engine::GetCharPressed() {
    s_CharInputEnabled = true;
    if (s_CharQueue.empty())
        return 0;

    unsigned int c = s_CharQueue.front();
    s_CharQueue.pop();

    s_CharInputEnabled = false;
    while (!s_CharQueue.empty())
        s_CharQueue.pop();
    return c;
}

bool Engine::IsMouseDown(const int button) { return s_MouseButtons[button]; }
bool Engine::IsMousePressedOnce(const int button) {
    return s_MouseButtons[button] && !s_PrevMouseButtons[button];
}
bool Engine::IsMouseReleased(const int button) {
    return !s_MouseButtons[button] && s_PrevMouseButtons[button];
}
glm::vec2 Engine::GetMousePos() {
    double x = 0;
    double y = 0;
    glfwGetCursorPos(s_Window, &x, &y);
    return {x, y};
}

glm::vec2 Engine::GetScreenToWorld2D(const glm::vec2 &screenPos) {
    glm::mat4 view = s_Camera2D.GetViewMatrix();
    glm::mat4 inv = glm::inverse(s_Projection2D * view);
    float x = ((2.0f * screenPos.x) / GetScreenWidth()) - 1.0f;
    float y = 1.0f - ((2.0f * screenPos.y) / GetScreenHeight());
    glm::vec4 ndc = {x, y, 0.0f, 1.0f};
    glm::vec4 world = inv * ndc;

    return {world.x, world.y};
}

CPL::Camera2D &Engine::GetCam2D() { return s_Camera2D; }

CPL::Camera3D &Engine::GetCam3D() { return s_Camera3D; }

GLFWwindow *Engine::GetWindow() { return s_Window; }

CPL::Shader &Engine::GetShader(const CPL::DrawModes &mode) {
    switch (mode) {
    case CPL::DrawModes::SHAPE_2D:
        return s_Shape2DShader;
        break;
    case CPL::DrawModes::TEXT:
        return s_TextShader;
        break;
    case CPL::DrawModes::TEX:
        return s_TextureShader;
        break;
    case CPL::DrawModes::SHAPE_2D_LIGHT:
        return s_LightShape2DShader;
        break;
    case CPL::DrawModes::TEX_LIGHT:
        return s_LightTextureShader;
        break;
    case CPL::DrawModes::SHAPE_3D:
        return s_Shape3DShader;
        break;
    case CPL::DrawModes::SHAPE_3D_LIGHT:
        return s_LightShape3DShader;
        break;
    }
    return s_Shape2DShader;
}
CPL::DrawModes &Engine::GetCurMode() { return s_CurrentDrawMode; }

CPL::Shader &Engine::GetScreenQuadShader() { return s_ScreenShader; }
CPL::Shader &Engine::GetCubeMapShader() { return s_CubeMapShader; }
CPL::Shader &Engine::GetDepthShader() { return s_DepthShader; }

CPL::Texture2D *Engine::GetWhiteTex() { return s_WhiteTex.get(); }
