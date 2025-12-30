#include "../CPLibrary/CPLibrary.h"
#include <string>

using namespace CPL;
PRIORITIZE_GPU_BY_VENDOR

std::unique_ptr<ShadowMap> g_ShadowMap;

std::unique_ptr<Texture2D> g_StoneTex;
std::unique_ptr<Texture2D> g_GrassTex;
std::unique_ptr<Texture2D> g_DirtTex;
std::unique_ptr<Texture2D> g_OakLogTex;
std::unique_ptr<Texture2D> g_OakLeaveTex;

std::unique_ptr<Texture2D> g_LogoTex;

std::unique_ptr<CubeMap> g_CubeMap;

std::vector<glm::vec3> g_BlockPos;
std::vector<glm::vec3> g_TreePos;

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

    // Shadows
    for (auto &p : g_BlockPos) {
        if (GetCam3D().frustum.IsCubeVisible(p, glm::vec3(0.1f))) {
            if (p.y == 9 * 0.2f) {
                Cube cube(p, glm::vec3(0.2f), WHITE);
                //cube.DrawDepth(depthShader);
            } else if (p.y >= 6 * 0.2f) {

            } else {
            }
        }
    }

    for (auto &p : g_TreePos) {
        for (int y = 1; y < 5; y++) {
            glm::vec3 stemPos(p.x, p.y + y * 0.2f, p.z);
            if (GetCam3D().frustum.IsCubeVisible(stemPos, glm::vec3(0.1f))) {
                Cube cube(stemPos, glm::vec3(0.2f), WHITE);
                cube.DrawDepth(depthShader);
            }
        }
        for (int x = -1; x < 2; x++) {
            for (int y = 3; y < 6; y++) {
                for (int z = -1; z < 2; z++) {
                    if (x == 0 && y < 5 && z == 0)
                        continue;
                    glm::vec3 leavePos(p.x + x * 0.2f, p.y + y * 0.2f,
                                       p.z + z * 0.2f);
                    if (GetCam3D().frustum.IsCubeVisible(leavePos,
                                                         glm::vec3(0.1f))) {
                        CubeTex cube(leavePos, glm::vec3(0.2f), WHITE);
                        cube.DrawDepth(depthShader, g_OakLeaveTex.get());
                    }
                }
            }
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

    // Draw cubes (if they are visible for the camera frustum)
    for (auto &p : g_BlockPos) {
        if (GetCam3D().frustum.IsCubeVisible(p, glm::vec3(0.1f))) {
            if (p.y == 9 * 0.2f) {
                DrawCubeTexAtlas(g_GrassTex.get(), p, glm::vec3(0.2f), WHITE);
            } else if (p.y >= 6 * 0.2f) {
                DrawCubeTex(g_DirtTex.get(), p, glm::vec3(0.2f), WHITE);
            } else {
                DrawCubeTexAtlas(g_StoneTex.get(), p, glm::vec3(0.2f), WHITE);
            }
        }
    }

    for (auto &p : g_TreePos) {
        for (int y = 1; y < 5; y++) {
            glm::vec3 stemPos(p.x, p.y + y * 0.2f, p.z);
            if (GetCam3D().frustum.IsCubeVisible(stemPos, glm::vec3(0.1f))) {
                DrawCubeTexAtlas(g_OakLogTex.get(), stemPos, glm::vec3(0.2f),
                                 WHITE);
            }
        }
        for (int x = -1; x < 2; x++) {
            for (int y = 3; y < 6; y++) {
                for (int z = -1; z < 2; z++) {
                    if (x == 0 && y < 5 && z == 0)
                        continue;
                    glm::vec3 leavePos(p.x + x * 0.2f, p.y + y * 0.2f,
                                       p.z + z * 0.2f);
                    if (GetCam3D().frustum.IsCubeVisible(leavePos,
                                                         glm::vec3(0.1f))) {
                        DrawCubeTexAtlas(g_OakLeaveTex.get(), leavePos,
                                         glm::vec3(0.2f), WHITE);
                    }
                }
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

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Set camera position
    GetCam3D().position = glm::vec3(0, 11 * 0.2f, 0);

    // Create shadow map with 4096x4096 resolution
    g_ShadowMap = std::make_unique<ShadowMap>(4096);
    // Load textures
    g_StoneTex = std::make_unique<Texture2D>("assets/images/stone.png",
                                             glm::vec2(48, 32),
                                             TextureFiltering::NEAREST);
    g_GrassTex = std::make_unique<Texture2D>("assets/images/grass_block.png",
                                             glm::vec2(48, 32),
                                             TextureFiltering::NEAREST);
    g_DirtTex = std::make_unique<Texture2D>(
        "assets/images/dirt.png", glm::vec2(16), TextureFiltering::NEAREST);
    g_OakLogTex = std::make_unique<Texture2D>("assets/images/oak_log.png",
                                              glm::vec2(48, 32),
                                              TextureFiltering::NEAREST);
    g_OakLeaveTex = std::make_unique<Texture2D>("assets/images/oak_leave.png",
                                                glm::vec2(48, 32),
                                                TextureFiltering::NEAREST);
    g_LogoTex = std::make_unique<Texture2D>(
        "assets/images/logo.png", glm::vec2(200), TextureFiltering::LINEAR);
    // Create cubemap
    g_CubeMap = std::make_unique<CubeMap>("assets/images/sky.png");

    // Init random block positions
    for (int x = -10; x < 10; x++) {
        for (int y = -10; y < 10; y++) {
            for (int z = -10; z < 10; z++) {
                g_BlockPos.emplace_back(
                    glm::vec3(x * 0.2f, y * 0.2f, z * 0.2f));
            }
        }
    }

    for (int i = 0; i < RandInt(5, 15); i++) {
        g_TreePos.emplace_back(glm::vec3(RandInt(-10 * 0.2f, 10 * 0.2f),
                                         9 * 0.2f,
                                         RandInt(-10 * 0.2f, 10 * 0.2f)));
    }

    // Loop
    while (!WindowShouldClose()) {
        MainLoop();
    }
    // Close window
    CloseWindow();
}
