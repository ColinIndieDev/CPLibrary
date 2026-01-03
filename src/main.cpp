#include "../CPLibrary/CPLibrary.h"
#include "Block.h"
#include "Chunk.h"
#include "TextureLoader.h"
#include "WorldGen.h"
#include <string>

using namespace CPL;
PRIORITIZE_GPU_BY_VENDOR

std::unique_ptr<ShadowMap> g_ShadowMap;
std::unique_ptr<CubeMap> g_CubeMap;

std::vector<glm::vec3> g_BlockPos;
std::vector<glm::vec3> g_TreePos;
std::vector<glm::vec3> g_GrassPos;
std::vector<glm::vec3> g_RedTulpPos;

std::map<BlockType, Texture2D *> g_TexAtlases;
std::vector<Chunk> g_Chunks;

glm::ivec2 mapSize(16, 16);
glm::ivec3 terrainSize(16, 64, 16);

void DrawGrass(const glm::vec3 pos) {
    // To fit the edges use pythagoras -> c² = a² + b² ( a, b = 0.2f) ~ 0.28f
    DrawPlaneTexRot(TextureLoader::GetBlockTex(TextureLoader::BlockType::GRASS),
                    pos, glm::vec3(90, 180, 45), glm::vec2(0.28f, 0.2f), WHITE);
    DrawPlaneTexRot(TextureLoader::GetBlockTex(TextureLoader::BlockType::GRASS),
                    pos, glm::vec3(90, 180, 135), glm::vec2(0.28f, 0.2f),
                    WHITE);
}

void DrawGrassDepth(const glm::vec3 pos, const Shader &depthShader) {
    PlaneTex plane1(pos, glm::vec3(90, 180, 45), glm::vec2(0.28f, 0.2f), WHITE);
    PlaneTex plane2(pos, glm::vec3(90, 180, 135), glm::vec2(0.28f, 0.2f),
                    WHITE);
    plane1.DrawDepth(depthShader, TextureLoader::GetBlockTex(
                                      TextureLoader::BlockType::GRASS));
    plane2.DrawDepth(depthShader, TextureLoader::GetBlockTex(
                                      TextureLoader::BlockType::GRASS));
}

void DrawRedTulp(const glm::vec3 pos) {
    // To fit the edges use pythagoras -> c² = a² + b² ( a, b = 0.2f) ~ 0.28f
    DrawPlaneTexRot(
        TextureLoader::GetBlockTex(TextureLoader::BlockType::RED_TULP), pos,
        glm::vec3(90, 180, 45), glm::vec2(0.28f, 0.2f), WHITE);
    DrawPlaneTexRot(
        TextureLoader::GetBlockTex(TextureLoader::BlockType::RED_TULP), pos,
        glm::vec3(90, 180, 135), glm::vec2(0.28f, 0.2f), WHITE);
}

void DrawRedTulpDepth(const glm::vec3 pos, const Shader &depthShader) {
    PlaneTex plane1(pos, glm::vec3(90, 180, 45), glm::vec2(0.28f, 0.2f), WHITE);
    PlaneTex plane2(pos, glm::vec3(90, 180, 135), glm::vec2(0.28f, 0.2f),
                    WHITE);
    plane1.DrawDepth(depthShader, TextureLoader::GetBlockTex(
                                      TextureLoader::BlockType::RED_TULP));
    plane2.DrawDepth(depthShader, TextureLoader::GetBlockTex(
                                      TextureLoader::BlockType::RED_TULP));
}

void UpdateCam() {
    float aspect = GetScreenWidth() / GetScreenHeight();
    Camera3D &cam = GetCam3D();
    cam.UpdateFrustum(aspect);

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

    if (IsKeyPressedOnce(KEY_ESCAPE))
        DestroyWindow();
}

glm::mat4 GetLightSpaceMatrix() {
    glm::vec3 lightPos(std::sin(GetTime() * 0.01f) * 5.0f,
                       (static_cast<float>(terrainSize.y) * 0.2f) + 3.2f,
                       std::cos(GetTime() * 0.01f) * 5.0f);
    glm::mat4 lightProjection =
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 25.0f);
    glm::mat4 lightView =
        glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    return lightProjection * lightView;
}

void HandleShadowMap() {
    glm::mat4 lightSpaceMatrix = GetLightSpaceMatrix();

    Shader &depthShader = Engine::GetDepthShader();
    depthShader.Use();
    depthShader.SetMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

    g_ShadowMap->BeginDepthPass(lightSpaceMatrix);

    for (auto &p : g_TreePos) {
        for (int y = 0; y < 4; y++) {
            glm::vec3 stemPos(p.x, p.y + (static_cast<float>(y) * 0.2f), p.z);
            Cube cube(stemPos, glm::vec3(0.2f), WHITE);
            cube.DrawDepth(depthShader);
        }
        for (int x = -1; x < 2; x++) {
            for (int y = 2; y < 5; y++) {
                for (int z = -1; z < 2; z++) {
                    if (x == 0 && y < 4 && z == 0)
                        continue;
                    glm::vec3 leavePos(p.x + (static_cast<float>(x) * 0.2f),
                                       p.y + (static_cast<float>(y) * 0.2f),
                                       p.z + (static_cast<float>(z) * 0.2f));
                    CubeTex cube(leavePos, glm::vec3(0.2f), WHITE);
                    cube.DrawDepthAtlas(
                        depthShader, TextureLoader::GetBlockTex(
                                         TextureLoader::BlockType::OAK_LEAVES));
                }
            }
        }
    }

    for (auto &p : g_GrassPos) {
        DrawGrassDepth(p + glm::vec3(0, 0.1f, 0.1f), depthShader);
    }
    for (auto &p : g_RedTulpPos) {
        DrawRedTulpDepth(p + glm::vec3(0, 0.1f, 0.1f), depthShader);
    }

    ShadowMap::EndDepthPass();
}

void ActivateShadowMap() {
    Shader &shader = GetShader(DrawModes::CUBE_TEX_LIGHT);
    shader.SetMatrix4fv("lightSpaceMatrix", GetLightSpaceMatrix());
    shader.SetInt("shadowMap", 1);

    g_ShadowMap->BindForReading(1);
}

void DrawTreeStem(const glm::vec3 &treePos, Chunk &chunk) {
    for (int y = 0; y < 4; y++) {
        glm::vec3 stemPos(treePos.x, treePos.y + (static_cast<float>(y)),
                          treePos.z);
        chunk.SetBlock(stemPos, BlockType::OAK_LOG);
    }
}
void DrawTreeLeaves(const glm::vec3 &treePos, Chunk &chunk) {
    for (int x = -1; x < 2; x++) {
        for (int y = 2; y < 5; y++) {
            for (int z = -1; z < 2; z++) {
                if (x == 0 && y < 4 && z == 0)
                    continue;
                glm::vec3 leavePos(treePos.x + (static_cast<float>(x)),
                                   treePos.y + (static_cast<float>(y)),
                                   treePos.z + (static_cast<float>(z)));
                chunk.SetBlock(leavePos, BlockType::OAK_LEAVES);
            }
        }
    }
}
void DrawTrees(Chunk &chunk) {
    for (auto &p : g_TreePos) {
        DrawTreeStem(p, chunk);
        DrawTreeLeaves(p, chunk);
    }
}
void DrawFoliage() {
    for (auto &p : g_GrassPos) {
        if (GetCam3D().frustum.IsCubeVisible(p, glm::vec3(0.1f))) {
            DrawGrass(p);
        }
    }
    for (auto &p : g_RedTulpPos) {
        if (GetCam3D().frustum.IsCubeVisible(p, glm::vec3(0.1f))) {
            DrawRedTulp(p);
        }
    }
}

void MainLoop() {
    UpdateCPL();

    UpdateCam();

    ClearBackground(SKY_BLUE);

    DrawCubeMap(g_CubeMap.get());

    EnableFaceCulling(true);

    BeginDraw(DrawModes::CUBE_TEX_LIGHT);

    SetShininess3D(32);

    SetDirLight3D(
        DirectionalLight(glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)),
                         glm::vec3(0.3f), glm::vec3(1.0f), glm::vec3(1.0f)));

    for (auto &c : g_Chunks) {
        c.Draw(GetShader(DrawModes::CUBE_TEX_LIGHT), g_TexAtlases);
    }

    EnableFaceCulling(false);

    BeginDraw(DrawModes::TEX, false);
    DrawTex2D(TextureLoader::GetCPLLogo(), {0, 0}, WHITE);

    BeginDraw(DrawModes::TEXT, false);

    DrawText({0, 0}, 0.5f,
             "X: " + std::to_string(GetCam3D().position.x) +
                 ", Y: " + std::to_string(GetCam3D().position.y) +
                 ", Z: " + std::to_string(GetCam3D().position.z),
             WHITE);

    ShowDetails();

    EndDraw();

    glfwSwapBuffers(GetWindow());
    glfwPollEvents();
}

int main() {
    InitWindow(1200, 800, "Welcome to CPL 3D");
    LockMouse(true);
    EnableVSync(false);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    GetCam3D().position = glm::vec3(0, 11 * 0.2f, 0);

    g_ShadowMap = std::make_unique<ShadowMap>(2048);

    g_CubeMap = std::make_unique<CubeMap>("assets/images/sky.png");

    TextureLoader::Init();

    Audio music = AudioManager::LoadAudio("assets/sounds/aria_math_music.mp3");
    AudioManager::PlayMusic(music);

    g_TexAtlases[BlockType::BEDROCK] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::BEDROCK);
    g_TexAtlases[BlockType::GRASS_BLOCK] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::GRASS_BLOCK);
    g_TexAtlases[BlockType::DIRT] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::DIRT);
    g_TexAtlases[BlockType::STONE] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::STONE);
    g_TexAtlases[BlockType::OAK_LOG] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::OAK_LOG);
    g_TexAtlases[BlockType::OAK_LEAVES] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::OAK_LEAVES);


    for (int x = 0; x < mapSize.x; x++) {
        for (int z = 0; z < mapSize.y; z++) {
            g_Chunks.emplace_back(glm::vec3(x, 0, z));
        }
    }

    for (auto &c : g_Chunks) {
        for (int y = 0; y < 64; y++) {
            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {
                    if (y == 0)
                        c.SetBlock(glm::vec3(x, y, z), BlockType::BEDROCK);
                    else if (y < 59)
                        c.SetBlock(glm::vec3(x, y, z), BlockType::STONE);
                    else if (y != 63)
                        c.SetBlock(glm::vec3(x, y, z), BlockType::DIRT);
                    else
                        c.SetBlock(glm::vec3(x, y, z), BlockType::GRASS_BLOCK);
                }
            }
        }
    }

    g_TreePos = WorldGen::GenTrees(terrainSize, 5);

    for (auto &c : g_Chunks) {
        DrawTrees(c);
    }

    for (auto &c : g_Chunks) {
        c.GenMesh();
    }

    while (!static_cast<bool>(WindowShouldClose())) {
        MainLoop();
    }
    CloseWindow();
}
