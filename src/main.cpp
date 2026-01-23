#include "../CPLibrary/CPLibrary.h"
#include "Game.h"

using namespace CPL;
PRIORITIZE_GPU_BY_VENDOR

int main() {
    std::string openGLVersion = "4.3";
    bool useOpenGLDebug = true;
    InitWindow(glm::ivec2(1600, 1000), "Low Budget Minecraft ($0)", useOpenGLDebug,
               openGLVersion); // Default 1200x800
    LockMouse(true);
    EnableVSync(false);

    Game game;
    game.Init();
    game.Run();

    CloseWindow();
}
