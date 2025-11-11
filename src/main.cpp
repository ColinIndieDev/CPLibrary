#include "../CPLibrary/CPLibrary.h"

using namespace CPL;
PRIORITIZE_GPU_BY_VENDOR

int main() {
    InitWindow(800, 600, "Welcome to CPL");

    std::vector<PointLight> lights;
    lights.push_back(PointLight({200, 200}, 450.0f, 2.0f, GREEN));
    lights.push_back(PointLight({500, 300}, 550.0f, 1.0f, RED));
    lights.push_back(PointLight({300, 400}, 350.0f, 2.0f, BLUE));

    while (!WindowShouldClose()) {
	UpdateCPL();

	BeginPostProcessing();

	ClearBackground(BLACK);

	BeginDrawing(SHAPE_2D_LIGHT, false);

        SetAmbientLight(0.0f);
	lights[0].position = GetMousePosition();
	AddPointLights(lights);

	DrawRectangle({0, 0}, {800, 600}, WHITE);
	DrawCircle(GetMousePosition(), 100.0f, RED);
	DrawTriangle({100, 100}, {200, 100}, LIME_GREEN);
	DrawLine({800, 0}, {0, 600}, PURPLE);

	BeginDrawing(TEXT, false);
	
	ShowDetails();

	EndPostProcessing();
	ApplyPostProcessing(PP_DEFAULT);

	EndDrawing();

	glfwSwapBuffers(window);
	glfwPollEvents();
    }
    CloseWindow();
}
