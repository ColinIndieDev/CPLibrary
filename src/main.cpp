#include "../CPLibrary/CPLibrary.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

using namespace CPL;
PRIORITIZE_GPU_BY_VENDOR

// Vector of 2D point lights
std::vector<PointLight> lights;

// Texture2D
std::unique_ptr<Texture2D> logoTex;
std::unique_ptr<Texture2D> blockTex;

// Tilemap
std::unique_ptr<Tilemap> tm;

void MainLoop() {
    // Update framework (fps, delta time, input etc.)
    UpdateCPL();

    // Start post processing (everything after is affected by PP)
    BeginPostProcessing();

    // Clear background with selected color
    ClearBackground(BLACK);

    // Start drawing shapes affected by lighting
    BeginDrawing(SHAPE_LIGHT, false);

    lights[0].position = GetMousePosition();
    // Add point lights to scene (you can do it once if the lights never change!)
    AddPointLights(lights);

    // Draw shapes
    DrawRectangle({0, 0}, {800, 600}, WHITE);
    DrawCircle(GetMousePosition(), 100.0f, RED);
    DrawTriangle({100, 100}, {200, 100}, LIME_GREEN);
    DrawLine({800, 0}, {0, 600}, PURPLE);

    // Start drawing textures affected by lighting
    BeginDrawing(TEXTURE_LIGHT, false);

    // Draw tilemap
    tm->Draw();

    // Draw texture
    DrawTexture2D(logoTex.get(), {GetScreenWidth() / 2, GetScreenHeight() / 2},
                  WHITE);

    // Start drawing text
    BeginDrawing(TEXT, false);

    ShowDetails();

    // End PP (everything after is not affected)
    EndPostProcessing();
    // Apply effects (nothing, blur, inversed colors etc.)
    ApplyPostProcessing(PP_DEFAULT);

    // End drawing
    EndDrawing();

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main() {
    // Create 800x600 window with title
    InitWindow(800, 600, "Welcome to CPL");
// Set the window icon
#ifndef __EMSCRIPTEN
    SetWindowIcon("assets/images/logo.png");
#endif

    // Add point lights to vector
    lights.push_back(PointLight({200, 200}, 450.0f, 2.0f, GREEN));
    lights.push_back(PointLight({500, 300}, 550.0f, 1.0f, RED));
    lights.push_back(PointLight({300, 400}, 350.0f, 2.0f, BLUE));

    // Init texture
    logoTex = std::make_unique<Texture2D>(
        Texture2D("assets/images/logo.png", {200, 200}, LINEAR));
    blockTex = std::make_unique<Texture2D>(
        Texture2D("assets/images/stone.jpg", {100, 100}, NEAREST));

    // Edit tilemap
    tm = std::make_unique<Tilemap>(Tilemap());
    tm->BeginEditing();

    for (int y = 3; y < 6; y++) {
        for (int x = 0; x < 8; x++) {
            tm->AddTile({x * 100, y * 100}, {100, 100}, blockTex.get());
        }
    }

    // Delete upper corner tile
    tm->DeleteTile({0, 300}, {100, 100}, blockTex.get());

    // Set ambient light
    SetAmbientLight(0);

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
