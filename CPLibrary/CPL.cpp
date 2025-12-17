#include "CPL.h"
#include "Audio.h"
#include "Shader.h"
#include "Text.h"
#include "shapes2D/Circle.h"
#include "shapes2D/Line.h"
#include "shapes2D/PointLight.h"
#include "shapes2D/Rectangle.h"
#include "shapes2D/ScreenQuad.h"
#include "shapes2D/Texture2D.h"
#include "shapes2D/Triangle.h"
#include "stb_image.h"
#include "timers/TimerManager.h"

namespace CPL {
Shader shapeShader;
Shader textShader;
Shader textureShader;
Shader lightShapeShader;
Shader lightTextureShader;
Shader screenShader;

ScreenQuad screenQuad;

void UpdateCPL() {
    UpdateInput();
    CalculateDeltaTime();
    CalculateFPS();
    TimerManager::Update(GetDeltaTime());
    AudioManager::Update();
}

void ShowDetails() {
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *version = glGetString(GL_VERSION);
    const std::string rendererString(reinterpret_cast<const char *>(renderer));
    const std::string vendorString(reinterpret_cast<const char *>(vendor));
    const std::string versionString(reinterpret_cast<const char *>(version));

    BeginDrawing(TEXT, false);
    const std::string fpsText = "FPS: " + std::to_string(GetFPS());
    DrawTextShadow({0, GetScreenHeight() - 130}, {2, 2}, 0.3, fpsText, WHITE,
                   DARK_GRAY);
    const std::string vendorText = "Vendor: " + vendorString;
    DrawTextShadow({0, GetScreenHeight() - 95}, {2, 2}, 0.3, vendorText, WHITE,
                   DARK_GRAY);
    const std::string rendererText = "GPU: " + rendererString;
    DrawTextShadow({0, GetScreenHeight() - 60}, {2, 2}, 0.3, rendererText,
                   WHITE, DARK_GRAY);
    const std::string versionText = "Version: " + versionString;
    DrawTextShadow({0, GetScreenHeight() - 25}, {2, 2}, 0.3, versionText, WHITE,
                   DARK_GRAY);
    EndDrawing();
}

bool CheckCollisionRects(const Rectangle &one, const Rectangle &two) {
    const bool collisionX = one.position.x + one.size.x >= two.position.x &&
                            two.position.x + two.size.x >= one.position.x;
    const bool collisionY = one.position.y + one.size.y >= two.position.y &&
                            two.position.y + two.size.y >= one.position.y;

    return collisionX && collisionY;
}
bool CheckCollisionCircleRect(const Circle &one, const Rectangle &two) {
    const glm::vec2 circleCenter = one.position;
    const glm::vec2 rectCenter = two.position + two.size * 0.5f;
    const glm::vec2 halfExtents = two.size * 0.5f;
    const glm::vec2 difference = circleCenter - rectCenter;
    const glm::vec2 clamped = glm::clamp(difference, -halfExtents, halfExtents);
    const glm::vec2 closest = rectCenter + clamped;
    const glm::vec2 delta = closest - circleCenter;

    return glm::length(delta) <= one.radius;
}
bool CheckCollisionVec2Rect(const glm::vec2 &one, const Rectangle &two) {
    return two.position.x < one.x && one.x < two.position.x + two.size.x &&
           two.position.y < one.y && one.y < two.position.y + two.size.y;
}
bool CheckCollisionCircleCircle(const Circle &one, const Circle &two) {
    const glm::vec2 dist = one.position - two.position;
    const float distanceSquared = dist.x * dist.x + dist.y * dist.y;
    const float radiusSum = one.radius + two.radius;
    return distanceSquared <= radiusSum * radiusSum;
}
bool CheckCollisionVec2Circle(const glm::vec2 &one, const Circle &two) {
    const glm::vec2 dist = one - two.position;
    const float distanceSquared = dist.x * dist.x + dist.y * dist.y;
    return distanceSquared <= two.radius * two.radius;
}

void InitWindow(const int width, const int height, const char *title) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;

    projection = glm::ortho(0.0f, static_cast<float>(width),
                            static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    InitShaders();
#ifdef __EMSCRIPTEN__
    Text::Init("/assets/fonts/default.ttf", "defaultFont", NEAREST);
#else
    Text::Init("assets/fonts/default.ttf", "defaultFont", NEAREST);
#endif
    AudioManager::Init();
    screenQuad.Init(width, height);
    InitCharPressed(window);

    // ----- For the font & 2D textures ----- //
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void SetWindowIcon(const std::string &filePath) {
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
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);
    } else {
        Logging::Log(2, "Failed to load icon");
    }
}

void CloseWindow() {
    glfwTerminate();
    AudioManager::Close();
}

void InitShaders() {
#ifdef __EMSCRIPTEN__
    shapeShader = Shader("/assets/shaders/shader_web.vert",
                         "/assets/shaders/shader_web.frag");
    textShader = Shader("/assets/shaders/text_web.vert",
                        "/assets/shaders/text_web.frag");
    textureShader = Shader("/assets/shaders/texture_web.vert",
                           "/assets/shaders/texture_web.frag");
    lightShapeShader = Shader("/assets/shaders/lightShader_web.vert",
                              "/assets/shaders/lightShader_web.frag");
    lightTextureShader = Shader("assets/shaders/lightTexture_web.vert",
                                "assets/shaders/lightTexture_web.frag");
    screenShader = Shader("/assets/shaders/screen_web.vert",
                          "/assets/shaders/screen_web.frag");
#else
    shapeShader =
        Shader("assets/shaders/shader.vert", "assets/shaders/shader.frag");
    textShader = Shader("assets/shaders/text.vert", "assets/shaders/text.frag");
    textureShader =
        Shader("assets/shaders/texture.vert", "assets/shaders/texture.frag");
    lightShapeShader = Shader("assets/shaders/lightShader.vert",
                              "assets/shaders/lightShader.frag");
    lightTextureShader = Shader("assets/shaders/lightTexture.vert",
                                "assets/shaders/lightTexture.frag");
    screenShader =
        Shader("assets/shaders/screen.vert", "assets/shaders/screen.frag");
#endif
}

void BeginDrawing(const DrawModes &mode, const bool mode2D) {
    Shader shader{};
    currentDrawMode = mode;
    if (mode == SHAPE_2D)
        shader = shapeShader;
    else if (mode == TEXT) {
        shader = textShader;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Text::Use("defaultFont");
    } else if (mode == TEXTURE_2D)
        shader = textureShader;
    else if (mode == SHAPE_2D_LIGHT)
        shader = lightShapeShader;
    else if (mode == TEXTURE_2D_LIGHT)
        shader = lightTextureShader;

    shader.Use();
    const glm::mat4 view = camera.GetViewMatrix();
    const glm::mat4 viewProjection = projection * view;
    shader.SetMatrix4fv("projection", mode2D ? viewProjection : projection);
}
void SetAmbientLight(const float strength) {
    lightShapeShader.Use();
    lightShapeShader.SetFloat("ambient", strength);

    lightTextureShader.Use();
    lightTextureShader.SetFloat("ambient", strength);

    Shader shader{};
    if (currentDrawMode == SHAPE_2D)
        shader = shapeShader;
    else if (currentDrawMode == TEXT) {
        shader = textShader;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Text::Use("defaultFont");
    } else if (currentDrawMode == TEXTURE_2D)
        shader = textureShader;
    else if (currentDrawMode == SHAPE_2D_LIGHT)
        shader = lightShapeShader;
    else if (currentDrawMode == TEXTURE_2D_LIGHT)
        shader = lightTextureShader;
    shader.Use();
}
void AddPointLights(const std::vector<PointLight> &lights) {
    lightShapeShader.Use();
    lightShapeShader.SetInt("numPointLights", lights.size());
    for (int i = 0; i < lights.size(); i++) {
        lightShapeShader.SetVector2f("pointLights[" + std::to_string(i) +
                                         "].position",
                                     lights[i].position);
        lightShapeShader.SetFloat(
            "pointLights[" + std::to_string(i) + "].radius", lights[i].radius);
        lightShapeShader.SetFloat("pointLights[" + std::to_string(i) +
                                      "].intensity",
                                  lights[i].intensity);
        lightShapeShader.SetColor(
            "pointLights[" + std::to_string(i) + "].color", lights[i].color);
    }

    lightTextureShader.Use();
    lightTextureShader.SetInt("numPointLights", lights.size());
    for (int i = 0; i < lights.size(); i++) { 
        lightTextureShader.SetVector2f("pointLights[" + std::to_string(i) +
                                           "].position",
                                       lights[i].position);
        lightTextureShader.SetFloat(
            "pointLights[" + std::to_string(i) + "].radius", lights[i].radius);
        lightTextureShader.SetFloat("pointLights[" + std::to_string(i) +
                                        "].intensity",
                                    lights[i].intensity);
        lightTextureShader.SetColor(
            "pointLights[" + std::to_string(i) + "].color", lights[i].color);
    }
    
    Shader shader{};
    if (currentDrawMode == SHAPE_2D)
        shader = shapeShader;
    else if (currentDrawMode == TEXT) {
        shader = textShader;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Text::Use("defaultFont");
    } else if (currentDrawMode == TEXTURE_2D)
        shader = textureShader;
    else if (currentDrawMode == SHAPE_2D_LIGHT)
        shader = lightShapeShader;
    else if (currentDrawMode == TEXTURE_2D_LIGHT)
        shader = lightTextureShader;
    shader.Use();
}

void BeginPostProcessing() { screenQuad.BeginUseScreen(); }
void EndPostProcessing() { screenQuad.EndUseScreen(); }
void ApplyPostProcessing(const PostProcessingModes &mode) {
    screenQuad.Draw(mode);
}
void ApplyPostProcessingCustom(const Shader &shader) {
    screenQuad.DrawCustom(shader);
}

void DrawTriangle(const glm::vec2 position, const glm::vec2 size,
                  const Color &color) {
    const auto triangle = Triangle(position, size, color);
    triangle.Draw(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                    : shapeShader,
                  true);
}
void DrawTriangleRotated(const glm::vec2 position, const glm::vec2 size,
                         const float angle, const Color &color) {
    const auto triangle = Triangle(position, size, color);
    triangle.rotationAngle = angle;
    triangle.Draw(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                    : shapeShader,
                  true);
}
void DrawTriangleOutline(const glm::vec2 position, const glm::vec2 size,
                         const Color &color) {
    const auto triangle = Triangle(position, size, color);
    triangle.Draw(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                    : shapeShader,
                  false);
}
void DrawTriangleRotOut(const glm::vec2 position, const glm::vec2 size,
                        const float angle, const Color &color) {
    const auto triangle = Triangle(position, size, color);
    triangle.rotationAngle = angle;
    triangle.Draw(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                    : shapeShader,
                  false);
}

void DrawRectangle(const glm::vec2 position, const glm::vec2 size,
                   const Color &color) {
    const auto rectangle = Rectangle(position, size, color);
    rectangle.Draw(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                     : shapeShader,
                   true);
}
void DrawRectangleRotated(const glm::vec2 position, const glm::vec2 size,
                          const float angle, const Color &color) {
    const auto rectangle = Rectangle(position, size, color);
    rectangle.rotationAngle = angle;
    rectangle.Draw(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                     : shapeShader,
                   true);
}
void DrawRectangleOutline(const glm::vec2 position, const glm::vec2 size,
                          const Color &color) {
    const auto rectangle = Rectangle(position, size, color);
    rectangle.Draw(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                     : shapeShader,
                   false);
}
void DrawRectangleRotOut(const glm::vec2 position, const glm::vec2 size,
                         const float angle, const Color &color) {
    const auto rectangle = Rectangle(position, size, color);
    rectangle.rotationAngle = angle;
    rectangle.Draw(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                     : shapeShader,
                   false);
}

void DrawCircle(const glm::vec2 position, const float radius,
                const Color &color) {
    const auto circle = Circle(position, radius, color);
    circle.Draw(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                  : shapeShader);
}
void DrawCircleOutline(const glm::vec2 position, const float radius,
                       const Color &color) {
    const auto circle = Circle(position, radius, color);
    circle.DrawOutline(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                         : shapeShader);
}

void DrawLine(const glm::vec2 startPos, const glm::vec2 endPos,
              const Color &color) {
    const auto line = Line(startPos, endPos, color);
    line.Draw(currentDrawMode == SHAPE_2D_LIGHT ? lightShapeShader
                                                : shapeShader);
}

void DrawTexture2D(Texture2D *texture, const glm::vec2 position,
                   const Color &color) {
    texture->position = position;
    texture->color = color;
    texture->Draw(currentDrawMode == TEXTURE_2D_LIGHT ? lightTextureShader
                                                      : textureShader);
}
void DrawTexture2DRotated(Texture2D *texture, const glm::vec2 position,
                          const float angle, const Color &color) {
    texture->position = position;
    texture->color = color;
    texture->rotationAngle = angle;
    texture->Draw(currentDrawMode == TEXTURE_2D_LIGHT ? lightTextureShader
                                                      : textureShader);
}
void DrawTex2DCpy(Texture2D texture, const glm::vec2 position,
                  const Color &color) {
    texture.position = position;
    texture.color = color;
    texture.Draw(currentDrawMode == TEXTURE_2D_LIGHT ? lightTextureShader
                                                     : textureShader);
}

void DrawText(const glm::vec2 position, const float scale,
              const std::string &text, const Color &color) {
    Text::DrawText(textShader, text, position, scale, color);
}
void DrawTextShadow(const glm::vec2 position, const glm::vec2 shadowOffset,
                    const float scale, const std::string &text,
                    const Color &color, const Color &shadowColor) {
    Text::DrawText(textShader, text,
                   {position.x + shadowOffset.x, position.y - shadowOffset.y},
                   scale, shadowColor);
    Text::DrawText(textShader, text, position, scale, color);
}
} // namespace CPL
