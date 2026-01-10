#include "Game.h"
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

void Game::Init() {
    const std::vector<std::string> skyPaths = {
        "assets/images/sky/nx.png",
        "assets/images/sky/px.png",
        "assets/images/sky/py.png",
        "assets/images/sky/ny.png",
        "assets/images/sky/nz.png",
        "assets/images/sky/pz.png",
    };
    const std::string skyPath = "assets/images/day.png";
    m_Skybox = std::make_unique<CubeMap>(skyPaths);

    m_ShadowMap = std::make_unique<ShadowMap>(2048);

    Audio music = AudioManager::LoadAudio("assets/sounds/aria_math_music.mp3");
    AudioManager::PlayMusic(music);

    TextureLoader::Init();
    
    m_InitAtlases();

    depthShader = Shader("assets/shaders/default/vert/cubeTexShader.vert",
                         "assets/shaders/default/frag/zPrepass.frag");
    m_WorldGen = std::make_unique<WorldGen>(RandInt(0, 999999999), viewDist,
                                            30, Chunk::s_Height, 60);
    m_WorldGen->Init();
    m_WorldGen->GenMap();

    m_SetSpawnPoint();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        UpdateCPL();

        m_Update();
        m_Draw();

        glfwSwapBuffers(GetWindow());
        glfwPollEvents();
    }
}

void Game::m_SetSpawnPoint() {
    const glm::ivec2 spawnPoint(0);

    const int height =
        m_WorldGen->GetTerrainHeight(spawnPoint.x, spawnPoint.y) + 3;
    GetCam3D().position.y = static_cast<float>(height) * 0.2f;
}

void UpdateControls() {
    float aspect = GetScreenWidth() / GetScreenHeight();
    Camera3D &cam = GetCam3D();
    cam.UpdateFrustum(aspect);

    float speed = 3.0f * GetDeltaTime();
    if (IsKeyDown(KEY_LEFT_CONTROL))
        speed *= 6;

    if (IsKeyDown(KEY_W))
        cam.position += speed * cam.front;
    if (IsKeyDown(KEY_S))
        cam.position -= speed * cam.front;
    if (IsKeyDown(KEY_A))
        cam.position -= glm::normalize(glm::cross(cam.front, cam.up)) * speed;
    if (IsKeyDown(KEY_D))
        cam.position += glm::normalize(glm::cross(cam.front, cam.up)) * speed;
    if (IsKeyDown(KEY_LEFT_SHIFT))
        cam.position.y -= speed;
    if (IsKeyDown(KEY_SPACE))
        cam.position.y += speed;

    if (IsKeyPressedOnce(KEY_ESCAPE))
        DestroyWindow();
}

void Game::m_Update() {
    UpdateControls();
    m_WorldGen->manager.ProcessFinishedChunks();
    m_WorldGen->manager.ProcessDirtyChunks(1);
    m_WorldGen->manager.UploadChunkMeshes();
    m_WorldGen->UpdateMap();
}

float NormalizeYaw(float yaw) {
    yaw = static_cast<float>(fmod(yaw, 360.0));

    if (yaw < 0)
        yaw += 360.0f;

    return yaw;
}

std::string GetCardinalDir(float yaw) {
    yaw = NormalizeYaw(yaw);

    if (yaw >= 337.5f || yaw < 22.5f) {
        return "N";
    }
    if (yaw >= 22.5f && yaw < 67.5f) {
        return "NE";
    }
    if (yaw >= 67.5f && yaw < 112.5f) {
        return "E";
    }
    if (yaw >= 112.5f && yaw < 157.5f) {
        return "SE";
    }
    if (yaw >= 157.5f && yaw < 202.5f) {
        return "S";
    }
    if (yaw >= 202.5f && yaw < 247.5f) {
        return "SW";
    }
    if (yaw >= 247.5f && yaw < 292.5f) {
        return "W";
    }
    return "NW";
}

void Game::m_Draw() {
    glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
    glm::mat4 lightProjection =
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 25.0f);
    glm::mat4 lightView =
        glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    Shader &depthShader = Engine::GetDepthShader();
    depthShader.Use();
    depthShader.SetMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);
    
    m_ShadowMap->BeginDepthPass(lightSpaceMatrix); 
    
    std::vector<Chunk *> sortedChunks;
    sortedChunks.reserve(m_WorldGen->manager.chunks.size());

    for (auto &[pos, chunk] : m_WorldGen->manager.chunks) {
        const float blockSize = 0.2f;
        const float halfSizeXZ = Chunk::s_Size * blockSize * 0.5f;
        const float halfSizeY = Chunk::s_Height * blockSize * 0.5f;
        const glm::vec3 center(glm::vec3(pos) * halfSizeXZ * 2.0f +
                               glm::vec3(halfSizeXZ, halfSizeY, halfSizeXZ));
        const glm::vec3 halfSize(halfSizeXZ, halfSizeY, halfSizeXZ);

        if (!GetCam3D().frustum.IsCubeVisible(center, halfSize) ||
            ChunkManager::OutOfRenderDist(pos, viewDist))
            continue;

        sortedChunks.push_back(&chunk);
    }

    std::sort(sortedChunks.begin(), sortedChunks.end(),
              [&](Chunk *a, Chunk *b) {
                  glm::vec3 aPos = a->GetPos();
                  glm::vec3 bPos = b->GetPos();

                  float da = glm::length2(aPos - GetCam3D().position);
                  float db = glm::length2(bPos - GetCam3D().position);

                  return da < db;
              });

    for (auto& chunk : sortedChunks) {
        chunk->DrawDepthShadow(depthShader, m_TexAtlases);
    }
    ShadowMap::EndDepthPass();

    ClearBackground(SKY_BLUE);

    float skyRot = GetTime() * 0.5f;
    DrawCubeMapRot(m_Skybox.get(), glm::vec3(0, skyRot, 0));

    EnableFaceCulling(true);

    BeginDraw(DrawModes::CUBE_TEX_LIGHT);

    SetShininess3D(6);

    DirectionalLight day(glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)),
                         Color(76.5, 76.5, 76.5, 255), glm::vec3(1.0f),
                         glm::vec3(1.0f));

    DirectionalLight night(glm::normalize(glm::vec3(0.0f)),
                           Color(0, 26, 103, 255), glm::vec3(1.0f),
                           glm::vec3(0.0f));

    SetDirLight3D(day);

    //m_ShadowMap->BindForReading(1);
    uint32_t chunksDrawn = m_WorldGen->manager.DrawChunks(
        GetShader(DrawModes::CUBE_TEX_LIGHT), depthShader, m_TexAtlases, viewDist);

    EnableFaceCulling(false);

    BeginDraw(DrawModes::TEX, false);

    Texture2D *logoTex = TextureLoader::GetCPLLogo();
    DrawTex2D(logoTex,
              {GetScreenWidth() - logoTex->size.x,
               GetScreenHeight() - logoTex->size.y},
              WHITE);

    BeginDraw(DrawModes::TEXT, false);

    int ready = 0;
    int meshLocal = 0;
    int dirty = 0;
    int none = 0;
    {
        std::lock_guard<std::mutex> lock(m_WorldGen->manager.m_ChunksMutex);
        for (auto &[pos, chunk] : m_WorldGen->manager.chunks) {
            switch (chunk.state) {
            case Chunk::MeshState::READY:
                ready++;
                break;
            case Chunk::MeshState::MESHED_LOCAL:
                meshLocal++;
                break;
            case Chunk::MeshState::DIRTY:
                dirty++;
                break;
            case Chunk::MeshState::NONE:
                none++;
                break;
            }
        }
    }

    DrawTextShadow({0, 60}, {3, -3}, 0.5f,
                   "Chunks - Ready: " + std::to_string(ready) +
                       " | Local: " + std::to_string(meshLocal) +
                       " | Dirty: " + std::to_string(dirty) +
                       " | None: " + std::to_string(none),
                   WHITE, DARK_GRAY);

    DrawTextShadow({0, 90}, {3, -3}, 0.5f,
                   "Chunks drawn: " + std::to_string(chunksDrawn) + " / " +
                       std::to_string(m_WorldGen->manager.chunks.size()),
                   WHITE, DARK_GRAY);

    DrawTextShadow(
        {0, 0}, {3, -3}, 0.5f,
        "X: " + std::to_string(static_cast<int>(GetCam3D().position.x * 5)) +
            ", Y: " +
            std::to_string(static_cast<int>(GetCam3D().position.y * 5)) +
            ", Z: " +
            std::to_string(static_cast<int>(GetCam3D().position.z * 5)),
        WHITE, DARK_GRAY);
    DrawTextShadow({0, 30}, {3, -3}, 0.5f,
                   "Seed: " + std::to_string(m_WorldGen->seed), WHITE,
                   DARK_GRAY);

    DrawTextShadow({GetScreenWidth() / 2, 0}, {7, -7}, 1.0f,
                   GetCardinalDir(GetCam3D().yaw), WHITE, DARK_GRAY);

    ShowDetails();

    EndDraw();
}

void Game::m_InitAtlases() {
    m_TexAtlases[BlockType::BEDROCK] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::BEDROCK);
    m_TexAtlases[BlockType::GRASS_BLOCK] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::GRASS_BLOCK);
    m_TexAtlases[BlockType::DIRT] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::DIRT);
    m_TexAtlases[BlockType::STONE] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::STONE);
    m_TexAtlases[BlockType::OAK_LOG] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::OAK_LOG);
    m_TexAtlases[BlockType::OAK_LEAVES] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::OAK_LEAVES);
    m_TexAtlases[BlockType::SNOW] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::SNOW);
}
