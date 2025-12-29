#include "../CPLibrary/CPLibrary.h"
#include <string>

using namespace CPL;
PRIORITIZE_GPU_BY_VENDOR

std::unique_ptr<ShadowMap> g_ShadowMap;
std::unique_ptr<Texture2D> g_GroundTex;
std::unique_ptr<Texture2D> g_LogoTex;
std::unique_ptr<CubeMap> g_CubeMap;
std::vector<glm::vec3> g_BlockPos;

void UpdateCam() {
    // Update frustum for frustum culling
    float aspect = GetScreenWidth() / GetScreenHeight();
    Camera3D &cam = GetCam3D();
    cam.UpdateFrustum(aspect);

    // Camera movement
    float speed = 1.0f * GetDeltaTime();
    if (IsKeyDown(KEY_LEFT_SHIFT))
        speed *= 6;

    if (IsKeyDown(KEY_W))
        cam.position += speed * cam.front;
    if (IsKeyDown(KEY_S))
        cam.position -= speed * cam.front;
    if (IsKeyDown(KEY_A))
        cam.position -= glm::normalize(glm::cross(cam.front, cam.up)) * speed;
    if (IsKeyDown(KEY_D))
        cam.position += glm::normalize(glm::cross(cam.front, cam.up)) * speed;

    // Destroy window with ESC
    if (IsKeyPressedOnce(KEY_ESCAPE))
        DestroyWindow();
}
void MainLoop() {
    // Update framework
    UpdateCPL();

    // Bools to configure if drawing spheres or cube textures
    bool drawCubes = true;
    bool drawSpheres = false;

    UpdateCam();

    // Shadow map configurations
    glm::vec3 lightPos(sin(GetTime()) * 5.0f, 5.0f, cos(GetTime()) * 5.0f);
    glm::mat4 lightProjection =
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 25.0f);
    glm::mat4 lightView =
        glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    Shader &depthShader = Engine::GetDepthShader();
    depthShader.Use();
    depthShader.SetMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

    g_ShadowMap->BeginDepthPass(lightSpaceMatrix);

    CubeTex ground({0.0f, -0.1f, 0.0f}, {10.0f, 0.2f, 10.0f}, WHITE);
    ground.DrawDepth(depthShader, g_GroundTex.get());

    if (drawCubes) {
        for (auto &p : g_BlockPos) {
            CubeTex cube(p, glm::vec3(0.2f), WHITE);
            cube.DrawDepth(depthShader, g_GroundTex.get());
        }
    }
    if (drawSpheres) {
        for (auto &p : g_BlockPos) {
            Sphere sphere(p, 0.1f, WHITE);
            sphere.DrawDepth(depthShader);
        }
    }

    g_ShadowMap->EndDepthPass();

    // Start rendering scene here
    ClearBackground(SKY_BLUE);

    // Draw cubemap (skybox)
    DrawCubeMap(g_CubeMap.get());

    // Enable face culling for cubes
    EnableFaceCulling(true);

    // Draw cubes with textures affected by light
    BeginDraw(DrawModes::CUBE_TEX_LIGHT);

    // Set shininess
    SetShininess3D(32);

    // Add directional light
    DirectionalLight dirLight(
        glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)), // direction
        glm::vec3(0.3f),                                // ambient
        glm::vec3(1.0f),                                // diffuse
        glm::vec3(1.0f)                                 // specular
    );
    SetDirLight3D(dirLight);

    // Activate shadows
    Shader &shader = GetShader(DrawModes::CUBE_TEX_LIGHT);
    shader.SetMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);
    shader.SetInt("shadowMap", 1);

    g_ShadowMap->BindForReading(1);

    // Draw ground
    DrawCubeTex(g_GroundTex.get(), {0.0f, -0.1f, 0.0f}, {10.0f, 0.2f, 10.0f},
                WHITE);

    // Draw cubes (if they are visible for the camera frustum)
    int drawnCubes = 0;
    if (drawCubes) {
        for (auto &p : g_BlockPos) {
            if (GetCam3D().frustum.IsCubeVisible(p, glm::vec3(0.1f))) {
                DrawCubeTex(g_GroundTex.get(), p, glm::vec3(0.2f), WHITE);
                drawnCubes++;
            }
        }
    }

    // Draw 3D shapes with single colors
    BeginDraw(DrawModes::SHAPE_3D);

    // Draw sphere simulating the light source where to create shadows from
    DrawSphere(lightPos, 0.8f, YELLOW);

    // Draw 3D shapes with single colors affected by light
    BeginDraw(DrawModes::SHAPE_3D_LIGHT);

    // Draw spheres (if they are visible for the camera frustum
    int drawnSpheres = 0;
    if (drawSpheres) {
        for (auto &p : g_BlockPos) {
            if (GetCam3D().frustum.IsSphereVisible(p, 0.1f)) {
                DrawSphere(p, 0.1f, WHITE);
                drawnSpheres++;
            }
        }
    }

    // Disable face culling (before making 2D stuff)
    EnableFaceCulling(false);

    // Draw 2D textures for UI
    BeginDraw(DrawModes::TEX, false);
    // Draw CPL logo
    DrawTex2D(g_LogoTex.get(), {0, 0}, WHITE);

    // Draw text
    BeginDraw(DrawModes::TEXT, false);
    const float fontSize = 1.0f;
    const std::string cubeText = "Drawn cubes: " + std::to_string(drawnCubes) +
                                 " / " + std::to_string(g_BlockPos.size());
    const std::string sphereText =
        "Drawn spheres: " + std::to_string(drawnSpheres) + " / " +
        std::to_string(g_BlockPos.size());
    const float textWidth =
        Text::GetTextSize(GetDefaultFont(), sphereText, fontSize).x;
    DrawTextShadow({GetScreenWidth() / 2 - textWidth / 2, 10}, {5, 5}, fontSize,
                   drawCubes ? cubeText : sphereText, WHITE, DARK_GRAY);
    // Display details
    ShowDetails();

    // End drawing
    EndDraw();

    // Swap buffers
    glfwSwapBuffers(GetWindow());
    glfwPollEvents();
}

int main() {
    // Create 1200x800 window with title
    InitWindow(1200, 800, "Welcome to CPL 3D");
    // Lock and hide mouse
    LockMouse(true);
    // Disable VSync
    EnableVSync(false);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Set camera position
    GetCam3D().position = glm::vec3(0, 0.4f, 0);

    // Create shadow map with 4096x4096 resolution
    g_ShadowMap = std::make_unique<ShadowMap>(4096);
    // Load textures
    g_GroundTex = std::make_unique<Texture2D>(
        "assets/images/stone.jpg", glm::vec2(100), TextureFiltering::NEAREST);
    g_LogoTex = std::make_unique<Texture2D>(
        "assets/images/logo.png", glm::vec2(200), TextureFiltering::LINEAR);
    // Create cubemap
    g_CubeMap = std::make_unique<CubeMap>("assets/images/sky.png");

    // Init random block positions
    for (int i = 0; i < 50; i++) {
        g_BlockPos.emplace_back(glm::vec3(
            RandFloat(-5, 5), RandFloat(0.1f, 0.1f), RandFloat(-5, 5)));
    }

    // Loop
    while (!WindowShouldClose()) {
        MainLoop();
    }
    // Close window
    CloseWindow();
}
