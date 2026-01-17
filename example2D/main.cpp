#include "../CPLibrary/CPLibrary.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

using namespace CPL;
PRIORITIZE_GPU_BY_VENDOR

// Vector of 2D point lights
std::vector<PointLight> g_Lights;

// Texture2D
std::unique_ptr<Texture2D> g_LogoTex;
std::unique_ptr<Texture2D> g_BlockTex;
std::unique_ptr<Texture2D> g_SmokeTex;

// Tilemap
std::unique_ptr<Tilemap> g_Tilemap;

// Particle system
ParticleSystem g_ParticleSystem(glm::vec2(0));

void MainLoop() {
    // Update framework (fps, delta time, input etc.)
    UpdateCPL();

    // If ESC is pressed, then close window & programm
    if (IsKeyPressedOnce(KEY_ESCAPE)) {
        DestroyWindow();
    }

    // Start post processing (everything after is affected by PP)
    BeginPostProcessing();

    // Clear background with selected color
    ClearBackground(BLACK);

    // Start drawing shapes affected by lighting
    BeginDraw(DrawModes::SHAPE_2D_LIGHT, false);

    // Add point lights to scene (you can do it once if the lights never
    // change!)
    AddPointLights2D(g_Lights);

    // Draw shapes
    DrawRect({0, 0}, {800, 600}, WHITE);
    DrawCircle(GetMousePos(), 100.0f, RED);
    DrawTriangle({100, 100}, {200, 100}, LIME_GREEN);
    DrawLine({800, 0}, {0, 600}, PURPLE);

    // Start drawing textures affected by lighting
    BeginDraw(DrawModes::TEX_LIGHT, false);

    // Draw tilemap
    g_Tilemap->Draw();

    // Set transparency of particle depending on its lifetime
    for (auto &p : g_ParticleSystem.particles) {
        p.color.a = (1 - p.curLifeTime / p.lifeTime) * 255;
    }

    // Update & draw particles
    g_ParticleSystem.Update();
    g_ParticleSystem.Draw();

    // Draw texture
    DrawTex2D(g_LogoTex.get(), {GetScreenWidth() / 2, GetScreenHeight() / 2},
              WHITE);

    // Start drawing text
    BeginDraw(DrawModes::TEXT, false);

    ShowDetails();

    // End PP (everything after is not affected)
    EndPostProcessing();
    // Apply effects (nothing, blur, inversed colors etc.)
    ApplyPostProcessing(PostProcessingModes::DEFAULT);

    // End drawing
    EndDraw();

    // Swap buffers
    glfwSwapBuffers(GetWindow());
    glfwPollEvents();
}

int main() {
    // Create 800x600 window with title
    InitWindow({800, 600}, "Welcome to CPL 2D");

    // Lock and hide mouse cursor
    LockMouse(true);

// Set the window icon
#ifndef __EMSCRIPTEN
    SetWindowIcon("assets/images/default/logo.png");
#endif

    // Add point lights to vector
    g_Lights.push_back(PointLight({200, 200}, 450.0f, 2.0f, GREEN));
    g_Lights.push_back(PointLight({500, 300}, 550.0f, 1.0f, RED));
    g_Lights.push_back(PointLight({300, 400}, 350.0f, 2.0f, BLUE));

    // Init global light
    GlobalLight globalLight = GlobalLight(0.3f, PURPLE);

    // Apply global light
    SetGlobalLight2D(globalLight);

    // Init texture
    g_LogoTex = std::make_unique<Texture2D>(Texture2D(
        "assets/images/default/logo.png", {200, 200}, TextureFiltering::LINEAR));
    g_BlockTex = std::make_unique<Texture2D>(Texture2D(
        "assets/images/example2D/grass.png", {100, 100}, TextureFiltering::NEAREST));
    g_SmokeTex = std::make_unique<Texture2D>(Texture2D(
        "assets/images/example2D/smoke.png", {300, 300}, TextureFiltering::LINEAR));

    // Edit tilemap
    g_Tilemap = std::make_unique<Tilemap>(Tilemap());
    g_Tilemap->BeginEditing();

    for (int y = 3; y < 6; y++) {
        for (int x = 0; x < 8; x++) {
            g_Tilemap->AddTile({x * 100, y * 100}, {100, 100}, g_BlockTex.get());
        }
    }

    // Delete upper corner tile
    g_Tilemap->DeleteTile({0, 300}, {100, 100}, g_BlockTex.get());

    // Set ambient light
    SetAmbientLight2D(0);

    // Set position of particle system
    g_ParticleSystem.pos = {GetScreenWidth() / 2, GetScreenHeight() / 2};

    // Set timer to spawn particles every 0.1 seconds
    Timer *particleSpawnTimer =
        TimerManager::AddTimer(0.2f, true, [](Timer *t) {
            glm::vec2 direction = {glm::cos(RandFloat(0, 2 * 3.14)) * 50,
                                   glm::sin(RandFloat(0, 2 * 3.14)) * 50};
            g_ParticleSystem.AddParticle(g_SmokeTex.get(), WHITE, RandFloat(0, 10), direction,
                           {0, 0});
        });

// Set Emscripten main loop if compiling to web else default
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(MainLoop, 0, true);
#else
    while (!WindowShouldClose()) {
        MainLoop();
    }
#endif

    // Close window
    CloseWindow();
}
