#include "Game.h"
#include "Block.h"
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

void Game::Init() {
    const std::vector<std::string> skyPaths = {
        "assets/images/sky/nx.png", "assets/images/sky/px.png",
        "assets/images/sky/py.png", "assets/images/sky/ny.png",
        "assets/images/sky/nz.png", "assets/images/sky/pz.png",
    };
    const std::string skyPath = "assets/images/day.png";
    m_Skybox = std::make_unique<CubeMap>(skyPaths);

    m_ShadowMap = std::make_unique<ShadowMap>(m_ShadowRes);

    Audio music = AudioManager::LoadAudio("assets/sounds/aria_math_music.mp3");
    AudioManager::PlayMusic(music);

    TextureLoader::Init();

    m_InitAtlases();

    depthShader = Shader("assets/shaders/default/vert/cubeTexShader.vert",
                         "assets/shaders/default/frag/zPrepass.frag");
    m_WorldGen = std::make_unique<WorldGen>(RandInt(0, 999999999), m_ViewDist,
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

        EndFrame();
    }
}

void Game::m_SetSpawnPoint() {
    const glm::ivec2 spawnPoint(0);

    const int height =
        m_WorldGen->GetTerrainHeight(spawnPoint.x, spawnPoint.y) + 3;
    GetCam3D().position.y = static_cast<float>(height) * 0.2f;
}

void Game::m_UpdateControls() {
    float aspect = GetScreenWidth() / GetScreenHeight();
    Camera3D &cam = GetCam3D();
    cam.UpdateFrustum(aspect);

    float speed = 3.0f * GetDeltaTime();
    if (IsKeyDown(KEY_LEFT_CONTROL))
        speed *= 6;

    if (IsKeyPressedOnce(KEY_U))
        m_UseShadows = !m_UseShadows;
    if (IsKeyPressedOnce(KEY_H)) {
        m_ShadowRes *= 2;
        m_ShadowMap = std::make_unique<ShadowMap>(m_ShadowRes);
    }
    if (IsKeyPressedOnce(KEY_N)) {
        m_ShadowRes /= 2;
        m_ShadowMap = std::make_unique<ShadowMap>(m_ShadowRes);
    }

    if (IsKeyPressedOnce(KEY_V))
        m_UseMSAA = !m_UseMSAA;

    EnableMSAA(m_UseMSAA);

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
    m_UpdateControls();
    m_WorldGen->manager.ProcessFinishedChunks();
    m_WorldGen->manager.ProcessDirtyChunks(1);
    m_WorldGen->manager.UploadChunkMeshes();

    m_WorldGen->transparentManager.ProcessFinishedChunks();
    m_WorldGen->transparentManager.ProcessDirtyChunks(1);
    m_WorldGen->transparentManager.UploadChunkMeshes();

    m_WorldGen->UpdateMap();

    m_WorldGen->manager.UpdateVisibleChunksDepth(m_ViewDist);
    m_WorldGen->transparentManager.UpdateVisibleChunksDepth(m_ViewDist);
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
    glm::mat4 lightSpaceMatrix(1);
    if (m_UseShadows) {
        constexpr float shadowRange = 40.0f;

        glm::vec3 lightDir =
            glm::normalize(glm::vec3(std::sin(GetTime() * 0.1f) * 0.3f, -1.0f,
                                     std::cos(GetTime() * 0.1f) * 0.3f));
        glm::vec3 center =
            GetCam3D().position + GetCam3D().front * (shadowRange * 0.5f);
        glm::vec3 lightPos = center - lightDir * shadowRange;
        glm::mat4 lightProjection =
            glm::ortho(-shadowRange, shadowRange, -shadowRange, shadowRange,
                       -shadowRange * 2.0f, shadowRange * 2.0f);
        glm::mat4 lightView =
            glm::lookAt(lightPos, center, glm::vec3(0.0f, 1.0f, 0.0f));
        float texelSize =
            (shadowRange * 2.0f) / static_cast<float>(m_ShadowRes);
        glm::vec4 centerLS = lightView * glm::vec4(center, 1.0f);
        centerLS.x = std::floor(centerLS.x / texelSize) * texelSize;
        centerLS.y = std::floor(centerLS.y / texelSize) * texelSize;
        glm::vec3 snappedCenter = glm::vec3(glm::inverse(lightView) * centerLS);
        lightView =
            glm::lookAt(lightPos, snappedCenter, glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;
    }
    m_WorldGen->manager.DrawShadowMapChunks(m_ShadowMap.get(), lightSpaceMatrix,
                                            m_TexAtlases, m_UseShadows);

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

    if (m_UseShadows) {
        Shader &shader = GetShader(DrawModes::CUBE_TEX_LIGHT);
        shader.SetMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);
        shader.SetInt("shadowMap", 1);
        m_ShadowMap->BindForReading(1);
    }

    m_WorldGen->manager.UpdateVisibleChunks(m_ViewDist);
    m_WorldGen->transparentManager.UpdateVisibleChunks(m_ViewDist);

    uint32_t chunksDrawn = m_WorldGen->manager.DrawChunks(
        GetShader(DrawModes::CUBE_TEX_LIGHT), depthShader, m_TexAtlases);

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    m_WorldGen->transparentManager.DrawChunks(
        GetShader(DrawModes::CUBE_TEX_LIGHT), depthShader, m_TexAtlases);
    glDepthMask(GL_TRUE);

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

    DrawTextShadow({0, 90}, {3, -3}, 0.5f, "--- Chunks ---", WHITE, DARK_GRAY);

    DrawTextShadow({0, 120}, {3, -3}, 0.5f,
                   "Chunks - Ready: " + std::to_string(ready) +
                       " | Local: " + std::to_string(meshLocal) +
                       " | Dirty: " + std::to_string(dirty) +
                       " | None: " + std::to_string(none),
                   WHITE, DARK_GRAY);

    DrawTextShadow({0, 150}, {3, -3}, 0.5f,
                   "Chunks drawn: " + std::to_string(chunksDrawn) + " / " +
                       std::to_string(m_WorldGen->manager.chunks.size()),
                   WHITE, DARK_GRAY);

    DrawTextShadow({0, 210}, {3, -3}, 0.5f, "--- Graphics ---", WHITE,
                   DARK_GRAY);
    DrawTextShadow({0, 240}, {3, -3}, 0.5f,
                   "Shadows: " + std::string(m_UseShadows ? "ON" : "OFF"),
                   WHITE, DARK_GRAY);
    DrawTextShadow({0, 270}, {3, -3}, 0.5f,
                   "Shadow resolution: " + std::to_string(m_ShadowRes), WHITE,
                   DARK_GRAY);
    DrawTextShadow({0, 300}, {3, -3}, 0.5f,
                   "Anti Aliasing: " + std::string(m_UseMSAA ? "MSAA" : "NONE"),
                   WHITE, DARK_GRAY);

    DrawTextShadow(
        {0, 360}, {3, -3}, 0.5f,
        "Stack used: " +
            std::to_string(static_cast<float>(Profiler::GetStackUsed()) /
                           static_cast<float>(Profiler::GetStackSize())) +
            "% (" + std::to_string(Profiler::GetStackUsed() / 1000000.0f) +
            " / " + std::to_string(Profiler::GetStackSize() / 1000000.0f) +
            " MB)",
        WHITE, DARK_GRAY);

    DrawTextShadow(
        {0, 390}, {3, -3}, 0.5f,
        "Heap used: " + std::to_string(Profiler::GetHeapUsed() / 1000000.0f) +
            " MB",
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
    m_TexAtlases[BlockType::SAND] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::SAND);
    m_TexAtlases[BlockType::WATER] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::WATER);
}
