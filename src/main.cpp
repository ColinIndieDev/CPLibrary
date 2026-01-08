#include "../CPLibrary/CPLibrary.h"
#include "Game.h"

using namespace CPL;
PRIORITIZE_GPU_BY_VENDOR

int main() {
    std::string openGLVersion = "3.3";
    bool useOpenGLDebug = false;
    InitWindow(glm::ivec2(1200, 800), "Welcome to CPL 3D", useOpenGLDebug, openGLVersion);
    LockMouse(true);
    EnableVSync(false);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Game game;
    game.Init();
    game.Run();

    CloseWindow();
}
