#include "Engine.h"
#include "Audio.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "Text.h"
#include "shapes2D/Circle.h"
#include "shapes2D/GlobalLight.h"
#include "shapes2D/Line.h"
#include "shapes2D/PointLight.h"
#include "shapes2D/Rectangle.h"
#include "shapes2D/ScreenQuad.h"
#include "shapes2D/Texture2D.h"
#include "shapes2D/Triangle.h"
#include "shapes3D/Cube.h"
#include "shapes3D/CubeMap.h"
#include "shapes3D/CubeTex.h"
#include "shapes3D/DirectionalLight.h"
#include "shapes3D/PointLight3D.h"
#include "stb_image.h"
#include "timers/TimerManager.h"
#include <vector>

unsigned int Engine::s_ScreenWidth;
unsigned int Engine::s_ScreenHeight;
glm::mat4 Engine::s_Projection2D;
glm::mat4 Engine::s_Projection3D;

CPL::Shader Engine::s_ShapeShader;
CPL::Shader Engine::s_TextShader;
CPL::Shader Engine::s_TextureShader;
CPL::Shader Engine::s_LightShapeShader;
CPL::Shader Engine::s_LightTextureShader;
CPL::Shader Engine::s_ScreenShader;

CPL::Shader Engine::s_CubeShader;
CPL::Shader Engine::s_CubeTexShader;
CPL::Shader Engine::s_LightCubeShader;
CPL::Shader Engine::s_LightCubeTexShader;
CPL::Shader Engine::s_CubeMapShader;
CPL::Shader Engine::s_DepthShader;

CPL::DrawModes Engine::s_CurrentDrawMode;

std::mt19937 Engine::s_Gen{std::random_device{}()};

std::unordered_map<int, bool> Engine::s_KeyStates;
std::unordered_map<int, bool> Engine::s_PrevKeyStates;
std::unordered_map<int, bool> Engine::s_MouseButtons;
std::unordered_map<int, bool> Engine::s_PrevMouseButtons;

GLFWwindow *Engine::s_Window;
std::queue<unsigned int> Engine::s_CharQueue;

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
    const bool collisionX = one.position.x + one.size.x >= two.position.x &&
                            two.position.x + two.size.x >= one.position.x;
    const bool collisionY = one.position.y + one.size.y >= two.position.y &&
                            two.position.y + two.size.y >= one.position.y;

    return collisionX && collisionY;
}
bool Engine::CheckCollisionCircleRect(const CPL::Circle &one,
                                      const CPL::Rectangle &two) {
    const glm::vec2 circleCenter = one.position;
    const glm::vec2 rectCenter = two.position + two.size * 0.5f;
    const glm::vec2 halfExtents = two.size * 0.5f;
    const glm::vec2 difference = circleCenter - rectCenter;
    const glm::vec2 clamped = glm::clamp(difference, -halfExtents, halfExtents);
    const glm::vec2 closest = rectCenter + clamped;
    const glm::vec2 delta = closest - circleCenter;

    return glm::length(delta) <= one.radius;
}
bool Engine::CheckCollisionVec2Rect(const glm::vec2 &one,
                                    const CPL::Rectangle &two) {
    return two.position.x < one.x && one.x < two.position.x + two.size.x &&
           two.position.y < one.y && one.y < two.position.y + two.size.y;
}
bool Engine::CheckCollisionCircles(const CPL::Circle &one,
                                   const CPL::Circle &two) {
    const glm::vec2 dist = one.position - two.position;
    const float distanceSquared = dist.x * dist.x + dist.y * dist.y;
    const float radiusSum = one.radius + two.radius;
    return distanceSquared <= radiusSum * radiusSum;
}
bool Engine::CheckCollisionVec2Circle(const glm::vec2 &one,
                                      const CPL::Circle &two) {
    const glm::vec2 dist = one - two.position;
    const float distanceSquared = dist.x * dist.x + dist.y * dist.y;
    return distanceSquared <= two.radius * two.radius;
}

void Engine::InitWindow(const int width, const int height, const char *title) {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    s_ScreenWidth = width;
    s_ScreenHeight = height;

    s_Projection2D = glm::ortho(0.0f, static_cast<float>(width),
                              static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    s_Projection3D = glm::perspective(
        glm::radians(s_Camera3D.fov),
        static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);

    s_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (s_Window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(s_Window);
    glfwSetFramebufferSizeCallback(s_Window, FramebufferSizeCallback);

    // Lock cursor (better for 3d viewing with camera)
    glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(s_Window, MouseCallback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

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
}

void Engine::SetWindowIcon(const std::string &filePath) {
    int width, height, channels;
    stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    int requiredComponents = 0;
    if (channels == 3)
        requiredComponents = 3;
    else if (channels == 4)
        requiredComponents = 4;
    GLFWimage images[1];
    images[0].pixels =
        stbi_load(filePath.c_str(), &images[0].width, &images[0].height,
                  nullptr, requiredComponents);
    if (images[0].pixels) {
        glfwSetWindowIcon(s_Window, 1, images);
        stbi_image_free(images[0].pixels);
    } else {
        Logging::Log(2, "Failed to load icon");
    }
}

void Engine::CloseWindow() {
    glfwTerminate();
    CPL::AudioManager::Close();
}

void Engine::InitShaders() {
#ifdef __EMSCRIPTEN__
    s_ShapeShader = CPL::Shader("/assets/shaders/shader_web.vert",
                              "/assets/shaders/shader_web.frag");
    s_TextShader = CPL::Shader("/assets/shaders/text_web.vert",
                             "/assets/shaders/text_web.frag");
    s_TextureShader = CPL::Shader("/assets/shaders/texture_web.vert",
                                "/assets/shaders/texture_web.frag");
    s_LightShapeShader = CPL::Shader("/assets/shaders/lightShader_web.vert",
                                   "/assets/shaders/lightShader_web.frag");
    s_LightTextureShader = CPL::Shader("assets/shaders/lightTexture_web.vert",
                                     "assets/shaders/lightTexture_web.frag");
    s_ScreenShader = CPL::Shader("/assets/shaders/screen_web.vert",
                               "/assets/shaders/screen_web.frag");
#else
    s_ShapeShader =
        CPL::Shader("assets/shaders/shader.vert", "assets/shaders/shader.frag");
    s_TextShader =
        CPL::Shader("assets/shaders/text.vert", "assets/shaders/text.frag");
    s_TextureShader = CPL::Shader("assets/shaders/texture.vert",
                                "assets/shaders/texture.frag");
    s_LightShapeShader = CPL::Shader("assets/shaders/lightShader.vert",
                                   "assets/shaders/lightShader.frag");
    s_LightTextureShader = CPL::Shader("assets/shaders/lightTexture.vert",
                                     "assets/shaders/lightTexture.frag");
    s_ScreenShader =
        CPL::Shader("assets/shaders/screen.vert", "assets/shaders/screen.frag");

    s_CubeShader = CPL::Shader("assets/shaders/cubeShader.vert",
                             "assets/shaders/cubeShader.frag");
    s_CubeTexShader = CPL::Shader("assets/shaders/cubeTexShader.vert",
                                "assets/shaders/cubeTexShader.frag");
    s_LightCubeShader = CPL::Shader("assets/shaders/lightCubeShader.vert",
                                  "assets/shaders/lightCubeShader.frag");
    s_CubeMapShader = CPL::Shader("assets/shaders/cubeMapShader.vert",
                                "assets/shaders/cubeMapShader.frag");
    s_LightCubeTexShader = CPL::Shader("assets/shaders/lightCubeTexShader.vert",
                                     "assets/shaders/lightCubeTexShader.frag");
    s_DepthShader = CPL::Shader("assets/shaders/depthShader.vert",
                              "assets/shaders/depthShader.frag");
#endif
}

void Engine::BeginDraw(const CPL::DrawModes &mode, const bool mode2D) {
    CPL::Shader shader{};
    s_CurrentDrawMode = mode;
    if (mode == CPL::DrawModes::SHAPE)
        shader = s_ShapeShader;
    else if (mode == CPL::DrawModes::TEXT) {
        shader = s_TextShader;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        CPL::Text::Use("defaultFont");
    } else if (mode == CPL::DrawModes::TEX)
        shader = s_TextureShader;
    else if (mode == CPL::DrawModes::SHAPE_LIGHT)
        shader = s_LightShapeShader;
    else if (mode == CPL::DrawModes::TEX_LIGHT)
        shader = s_LightTextureShader;
    else if (mode == CPL::DrawModes::CUBE)
        shader = s_CubeShader;
    else if (mode == CPL::DrawModes::CUBE_TEX)
        shader = s_CubeTexShader;
    else if (mode == CPL::DrawModes::CUBE_LIGHT)
        shader = s_LightCubeShader;
    else if (mode == CPL::DrawModes::CUBE_TEX_LIGHT)
        shader = s_LightCubeTexShader;

    shader.Use();
    if (mode == CPL::DrawModes::CUBE || mode == CPL::DrawModes::CUBE_LIGHT ||
        mode == CPL::DrawModes::CUBE_TEX ||
        mode == CPL::DrawModes::CUBE_TEX_LIGHT) {
        shader.SetMatrix4fv("projection",
                            s_Projection3D * s_Camera3D.GetViewMatrix());
        glEnable(GL_DEPTH_TEST);
    } else {
        const glm::mat4 view = s_Camera2D.GetViewMatrix();
        const glm::mat4 viewProjection = s_Projection2D * view;
        shader.SetMatrix4fv("projection",
                            mode2D ? viewProjection : s_Projection2D);
        glDisable(GL_DEPTH_TEST);
    }
}
void Engine::ResetShader() {
    CPL::Shader shader{};
    if (s_CurrentDrawMode == CPL::DrawModes::SHAPE)
        shader = s_ShapeShader;
    else if (s_CurrentDrawMode == CPL::DrawModes::TEXT) {
        shader = s_TextShader;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        CPL::Text::Use("defaultFont");
    } else if (s_CurrentDrawMode == CPL::DrawModes::TEX)
        shader = s_TextureShader;
    else if (s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT)
        shader = s_LightShapeShader;
    else if (s_CurrentDrawMode == CPL::DrawModes::TEX_LIGHT)
        shader = s_LightTextureShader;
    else if (s_CurrentDrawMode == CPL::DrawModes::CUBE)
        shader = s_CubeShader;
    else if (s_CurrentDrawMode == CPL::DrawModes::CUBE_TEX)
        shader = s_CubeTexShader;
    else if (s_CurrentDrawMode == CPL::DrawModes::CUBE_LIGHT)
        shader = s_LightCubeShader;
    else if (s_CurrentDrawMode == CPL::DrawModes::CUBE_TEX_LIGHT)
        shader = s_LightCubeTexShader;
    shader.Use();
}
void Engine::SetAmbientLight2D(const float strength) {
    s_LightShapeShader.Use();
    s_LightShapeShader.SetFloat("ambient", strength);

    s_LightTextureShader.Use();
    s_LightTextureShader.SetFloat("ambient", strength);

    ResetShader();
}
void Engine::SetGlobalLight2D(const CPL::GlobalLight &light) {
    s_LightShapeShader.Use();
    s_LightShapeShader.SetFloat("globalLight.intensity", light.intensity);
    s_LightShapeShader.SetColor("globalLight.color", light.color);

    s_LightTextureShader.Use();
    s_LightTextureShader.SetFloat("globalLight.intensity", light.intensity);
    s_LightTextureShader.SetColor("globalLight.color", light.color);

    ResetShader();
}

void Engine::AddPointLights2D(const std::vector<CPL::PointLight> &lights) {
    s_LightShapeShader.Use();
    s_LightShapeShader.SetInt("numPointLights", lights.size());
    for (int i = 0; i < lights.size(); i++) {
        s_LightShapeShader.SetVector2f("pointLights[" + std::to_string(i) +
                                         "].position",
                                     lights[i].position);
        s_LightShapeShader.SetFloat(
            "pointLights[" + std::to_string(i) + "].radius", lights[i].radius);
        s_LightShapeShader.SetFloat("pointLights[" + std::to_string(i) +
                                      "].intensity",
                                  lights[i].intensity);
        s_LightShapeShader.SetColor(
            "pointLights[" + std::to_string(i) + "].color", lights[i].color);
    }

    s_LightTextureShader.Use();
    s_LightTextureShader.SetInt("numPointLights", lights.size());
    for (int i = 0; i < lights.size(); i++) {
        s_LightTextureShader.SetVector2f("pointLights[" + std::to_string(i) +
                                           "].position",
                                       lights[i].position);
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
    s_LightCubeShader.Use();
    s_LightCubeShader.SetFloat("shininess", shininess);

    s_LightCubeTexShader.Use();
    s_LightCubeTexShader.SetFloat("shininess", shininess);

    ResetShader();
}
void Engine::AddPointLights3D(const std::vector<CPL::PointLight3D> &lights) {
    s_LightCubeShader.Use();
    s_LightCubeShader.SetInt("numPointLights", lights.size());
    for (int i = 0; i < lights.size(); i++) {
        s_LightCubeShader.SetVector3f("pointLights[" + std::to_string(i) +
                                        "].position",
                                    lights[i].position);
        s_LightCubeShader.SetFloat("pointLights[" + std::to_string(i) +
                                     "].intensity",
                                 lights[i].intensity);
        s_LightCubeShader.SetFloat("pointLights[" + std::to_string(i) +
                                     "].constant",
                                 lights[i].constant);
        s_LightCubeShader.SetFloat(
            "pointLights[" + std::to_string(i) + "].linear", lights[i].linear);
        s_LightCubeShader.SetFloat("pointLights[" + std::to_string(i) +
                                     "].quadratic",
                                 lights[i].quadratic);
        s_LightCubeShader.SetColor("pointLights[" + std::to_string(i) + "].color",
                                 lights[i].color);
    }

    s_LightCubeTexShader.Use();
    s_LightCubeTexShader.SetInt("numPointLights", lights.size());
    for (int i = 0; i < lights.size(); i++) {
        s_LightCubeTexShader.SetVector3f("pointLights[" + std::to_string(i) +
                                           "].position",
                                       lights[i].position);
        s_LightCubeTexShader.SetFloat("pointLights[" + std::to_string(i) +
                                        "].intensity",
                                    lights[i].intensity);
        s_LightCubeTexShader.SetFloat("pointLights[" + std::to_string(i) +
                                        "].constant",
                                    lights[i].constant);
        s_LightCubeTexShader.SetFloat(
            "pointLights[" + std::to_string(i) + "].linear", lights[i].linear);
        s_LightCubeTexShader.SetFloat("pointLights[" + std::to_string(i) +
                                        "].quadratic",
                                    lights[i].quadratic);
        s_LightCubeTexShader.SetColor(
            "pointLights[" + std::to_string(i) + "].color", lights[i].color);
    }

    ResetShader();
}
void Engine::SetDirLight3D(const CPL::DirectionalLight &light) {
    s_LightCubeShader.Use();
    s_LightCubeShader.SetVector3f("viewPos", s_Camera3D.position);
    s_LightCubeShader.SetVector3f("dirLight.direction",
                                glm::vec3(-0.2f, -1.0f, -0.3f));
    s_LightCubeShader.SetVector3f("dirLight.ambient", glm::vec3(0.3f));
    s_LightCubeShader.SetVector3f("dirLight.diffuse", glm::vec3(1, 1, 0.9f));
    s_LightCubeShader.SetVector3f("dirLight.specular", glm::vec3(1));

    s_LightCubeTexShader.Use();
    s_LightCubeTexShader.SetVector3f("viewPos", s_Camera3D.position);
    s_LightCubeTexShader.SetVector3f("dirLight.direction",
                                   glm::vec3(-0.2f, -1.0f, -0.3f));
    s_LightCubeTexShader.SetVector3f("dirLight.ambient", glm::vec3(0.3f));
    s_LightCubeTexShader.SetVector3f("dirLight.diffuse", glm::vec3(1, 1, 0.9f));
    s_LightCubeTexShader.SetVector3f("dirLight.specular", glm::vec3(1));

    ResetShader();
}

void Engine::BeginPostProcessing() { s_ScreenQuad.BeginUseScreen(); }
void Engine::EndPostProcessing() { s_ScreenQuad.EndUseScreen(); }
void Engine::ApplyPostProcessing(const CPL::PostProcessingModes &mode) {
    s_ScreenQuad.Draw(static_cast<int>(mode));
}
void Engine::ApplyPostProcessingCustom(const CPL::Shader &shader) {
    s_ScreenQuad.DrawCustom(shader);
}

void Engine::DrawTriangle(const glm::vec2 pos, const glm::vec2 size,
                          const CPL::Color &color) {
    const auto triangle = CPL::Triangle(pos, size, color);
    triangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT
                      ? s_LightShapeShader
                      : s_ShapeShader,
                  true);
}
void Engine::DrawTriangleRot(const glm::vec2 pos, const glm::vec2 size,
                             const float angle, const CPL::Color &color) {
    const auto triangle = CPL::Triangle(pos, size, color);
    triangle.rotationAngle = angle;
    triangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT
                      ? s_LightShapeShader
                      : s_ShapeShader,
                  true);
}
void Engine::DrawTriangleOut(const glm::vec2 pos, const glm::vec2 size,
                             const CPL::Color &color) {
    const auto triangle = CPL::Triangle(pos, size, color);
    triangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT
                      ? s_LightShapeShader
                      : s_ShapeShader,
                  false);
}
void Engine::DrawTriangleRotOut(const glm::vec2 pos, const glm::vec2 size,
                                const float angle, const CPL::Color &color) {
    const auto triangle = CPL::Triangle(pos, size, color);
    triangle.rotationAngle = angle;
    triangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT
                      ? s_LightShapeShader
                      : s_ShapeShader,
                  false);
}

void Engine::DrawRect(const glm::vec2 pos, const glm::vec2 size,
                      const CPL::Color &color) {
    const auto rectangle = CPL::Rectangle(pos, size, color);
    rectangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT
                       ? s_LightShapeShader
                       : s_ShapeShader,
                   true);
}
void Engine::DrawRectRot(const glm::vec2 pos, const glm::vec2 size,
                         const float angle, const CPL::Color &color) {
    const auto rectangle = CPL::Rectangle(pos, size, color);
    rectangle.rotationAngle = angle;
    rectangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT
                       ? s_LightShapeShader
                       : s_ShapeShader,
                   true);
}
void Engine::DrawRectOut(const glm::vec2 pos, const glm::vec2 size,
                         const CPL::Color &color) {
    const auto rectangle = CPL::Rectangle(pos, size, color);
    rectangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT
                       ? s_LightShapeShader
                       : s_ShapeShader,
                   false);
}
void Engine::DrawRectRotOut(const glm::vec2 pos, const glm::vec2 size,
                            const float angle, const CPL::Color &color) {
    const auto rectangle = CPL::Rectangle(pos, size, color);
    rectangle.rotationAngle = angle;
    rectangle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT
                       ? s_LightShapeShader
                       : s_ShapeShader,
                   false);
}

void Engine::DrawCircle(const glm::vec2 pos, const float radius,
                        const CPL::Color &color) {
    const auto circle = CPL::Circle(pos, radius, color);
    circle.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT
                    ? s_LightShapeShader
                    : s_ShapeShader);
}
void Engine::DrawCircleOut(const glm::vec2 pos, const float radius,
                           const CPL::Color &color) {
    const auto circle = CPL::Circle(pos, radius, color);
    circle.DrawOutline(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT
                           ? s_LightShapeShader
                           : s_ShapeShader);
}

void Engine::DrawLine(const glm::vec2 startPos, const glm::vec2 endPos,
                      const CPL::Color &color) {
    const auto line = CPL::Line(startPos, endPos, color);
    line.Draw(s_CurrentDrawMode == CPL::DrawModes::SHAPE_LIGHT ? s_LightShapeShader
                                                             : s_ShapeShader);
}

void Engine::DrawTex2D(CPL::Texture2D *tex, const glm::vec2 pos,
                       const CPL::Color &color) {
    tex->position = pos;
    tex->color = color;
    tex->Draw(s_CurrentDrawMode == CPL::DrawModes::TEX_LIGHT ? s_LightTextureShader
                                                           : s_TextureShader);
}
void Engine::DrawTex2DRot(CPL::Texture2D *tex, const glm::vec2 pos,
                          const float angle, const CPL::Color &color) {
    tex->position = pos;
    tex->color = color;
    tex->rotationAngle = angle;
    tex->Draw(s_CurrentDrawMode == CPL::DrawModes::TEX_LIGHT ? s_LightTextureShader
                                                           : s_TextureShader);
}

void Engine::DrawText(const glm::vec2 pos, const float scale,
                      const std::string &text, const CPL::Color &color) {
    CPL::Text::DrawText(s_TextShader, text, pos, scale, color);
}
void Engine::DrawTextShadow(const glm::vec2 pos, const glm::vec2 shadowOff,
                            const float scale, const std::string &text,
                            const CPL::Color &color,
                            const CPL::Color &shadowColor) {
    CPL::Text::DrawText(s_TextShader, text,
                        {pos.x + shadowOff.x, pos.y - shadowOff.y}, scale,
                        shadowColor);
    CPL::Text::DrawText(s_TextShader, text, pos, scale, color);
}

void Engine::DrawCube(const glm::vec3 pos, const glm::vec3 size,
                      const CPL::Color &color) {
    const auto cube = CPL::Cube(pos, size, color);
    cube.Draw(s_CurrentDrawMode == CPL::DrawModes::CUBE_LIGHT ? s_LightCubeShader
                                                            : s_CubeShader);
}

void Engine::DrawCubeTex(CPL::Texture2D *tex, const glm::vec3 pos,
                         const glm::vec3 size, const CPL::Color &color) {
    const auto cubeTex = CPL::CubeTex(pos, size, color);
    cubeTex.Draw(s_CurrentDrawMode == CPL::DrawModes::CUBE_TEX_LIGHT
                     ? s_LightCubeTexShader
                     : s_CubeTexShader,
                 tex);
}

void Engine::DrawCubeMap(CPL::CubeMap *map) {
    glDepthMask(GL_FALSE);
    map->Draw(s_CubeMapShader);
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

void Engine::MouseCallback(GLFWwindow *window, double xPosIn, double yPosIn) {
    float xPos = static_cast<float>(xPosIn);
    float yPos = static_cast<float>(yPosIn);

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

    if (s_Camera3D.pitch > 89.0f)
        s_Camera3D.pitch = 89.0f;
    if (s_Camera3D.pitch < -89.0f)
        s_Camera3D.pitch = -89.0f;

    glm::vec3 front;
    front.x =
        cos(glm::radians(s_Camera3D.yaw)) * cos(glm::radians(s_Camera3D.pitch));
    front.y = sin(glm::radians(s_Camera3D.pitch));
    front.z =
        sin(glm::radians(s_Camera3D.yaw)) * cos(glm::radians(s_Camera3D.pitch));
    s_Camera3D.front = glm::normalize(front);
}

void Engine::CharCallback(GLFWwindow *window, unsigned int codepoint) {
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
void Engine::EnableVSync(const bool enabled) { glfwSwapInterval(enabled); }
void Engine::EnableFaceCulling(const bool enabled) {
    if (enabled) {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }
}
int Engine::WindowShouldClose() { return glfwWindowShouldClose(s_Window); }

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
        s_MouseButtons[button] = glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
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
unsigned int Engine::GetCharPressed() {
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
    double x, y;
    glfwGetCursorPos(s_Window, &x, &y);
    return {x, y};
}

glm::vec2 Engine::GetScreenToWorld2D(const glm::vec2 &screenPos) {
    glm::mat4 view = s_Camera2D.GetViewMatrix();
    glm::mat4 inv = glm::inverse(s_Projection2D * view);
    float x = (2.0f * screenPos.x) / GetScreenWidth() - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y) / GetScreenHeight();
    glm::vec4 ndc = {x, y, 0.0f, 1.0f};
    glm::vec4 world = inv * ndc;

    return glm::vec2(world.x, world.y);
}

CPL::Camera2D &Engine::GetCam2D() { return s_Camera2D; }

CPL::Camera3D &Engine::GetCam3D() { return s_Camera3D; }

GLFWwindow *Engine::GetWindow() { return s_Window; }

CPL::Shader &Engine::GetShader(const CPL::DrawModes &mode) {
    if (mode == CPL::DrawModes::SHAPE)
        return s_ShapeShader;
    if (mode == CPL::DrawModes::TEXT)
        return s_TextShader;
    if (mode == CPL::DrawModes::TEX)
        return s_TextureShader;
    if (mode == CPL::DrawModes::SHAPE_LIGHT)
        return s_LightShapeShader;
    if (mode == CPL::DrawModes::TEX_LIGHT)
        return s_LightTextureShader;
    if (mode == CPL::DrawModes::CUBE)
        return s_CubeShader;
    if (mode == CPL::DrawModes::CUBE_TEX)
        return s_CubeTexShader;
    if (mode == CPL::DrawModes::CUBE_LIGHT)
        return s_LightCubeShader;
    if (mode == CPL::DrawModes::CUBE_TEX_LIGHT)
        return s_LightCubeTexShader;

    return s_ShapeShader;
}
CPL::DrawModes &Engine::GetCurMode() { return s_CurrentDrawMode; }

CPL::Shader &Engine::GetScreenQuadShader() { return s_ScreenShader; }
CPL::Shader &Engine::GetCubeMapShader() { return s_CubeMapShader; }
CPL::Shader &Engine::GetDepthShader() { return s_DepthShader; }
