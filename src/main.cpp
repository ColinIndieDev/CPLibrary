#include "../CPLibrary/CPLibrary.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

using namespace CPL;
PRIORITIZE_GPU_BY_VENDOR

// Vector of 2D point lights
std::vector<PointLight> lights;

void MainLoop() {
    // Update framework (fps, delta time, input etc.)
    UpdateCPL();

    // Start post processing (everything after is affected by PP)
    BeginPostProcessing();

    // Clear background with selected color
    ClearBackground(BLACK);

    // Start drawing shapes affected by lighting
    BeginDrawing(SHAPE_2D_LIGHT, false);

    // Set ambient strength
    SetAmbientLight(0.0f);
    lights[0].position = GetMousePosition();
    // Add point lights to scene
    AddPointLights(lights);

    // Draw shapes
    DrawRectangle({0, 0}, {800, 600}, WHITE);
    DrawCircle(GetMousePosition(), 100.0f, RED);
    DrawTriangle({100, 100}, {200, 100}, LIME_GREEN);
    DrawLine({800, 0}, {0, 600}, PURPLE);

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
