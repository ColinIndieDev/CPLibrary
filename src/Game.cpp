#include "Game.h"
#include "Block.h"
#include "Chunk.h"
#include "glm/fwd.hpp"
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

void Game::Init() {
    /*
    const std::vector<std::string> skyPaths = {
        "assets/images/sky/nx.png", "assets/images/sky/px.png",
        "assets/images/sky/py.png", "assets/images/sky/ny.png",
        "assets/images/sky/nz.png", "assets/images/sky/pz.png",
    };
    */

    const std::vector<std::string> skyPaths = {
        "assets/images/sky_color.png", "assets/images/sky_color.png",
        "assets/images/sky_color.png", "assets/images/sky_color.png",
        "assets/images/sky_color.png", "assets/images/sky_color.png",
    };

    m_Skybox = std::make_unique<CubeMap>(skyPaths);

    m_ShadowMap = std::make_unique<ShadowMap>(m_ShadowRes);

    Audio music = AudioManager::LoadAudio("assets/sounds/aria_math_music.mp3");
    AudioManager::PlayMusic(music);

    TextureLoader::Init();

    m_InitAtlases();

    Text::Init("assets/fonts/minecraft.ttf", "minecraftFont",
               TextureFiltering::NEAREST);

    m_DepthShader = Shader("assets/shaders/default/vert/cubeTexShader.vert",
                           "assets/shaders/default/frag/zPrepass.frag");

    uint32_t seed = RandInt(0, 999999999);

    m_WorldGen =
        std::make_unique<WorldGen>(seed, m_ViewDist, 30, Chunk::s_Height, 60);
    m_WorldGen->Init();
    m_WorldGen->GenMap();

    m_SetSpawnPoint();

    for (int i = 0; i < 200; i++) {
        glm::vec3 cloudPos(RandFloat(-100, 100), 150 * 0.2f,
                           RandFloat(-100, 100));
        glm::vec3 pos(GetCam3D().position.x + cloudPos.x, cloudPos.y,
                      GetCam3D().position.z + cloudPos.z);
        m_Clouds.emplace_back(pos,
                              glm::vec3(RandFloat(1, 10), 1, RandFloat(1, 10)));
    }
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
    int height = 0;
    int worldX = 0;
    int worldZ = 0;
    while (height < 65) {
        worldX = RandInt(-1000, 1000);
        worldZ = RandInt(-1000, 1000);

        int baseHeight = m_WorldGen->GetTerrainHeight(worldX, worldZ);
        std::pair<bool, float> riverData =
            m_WorldGen->GenRivers(worldX, worldZ);
        bool isRiver = riverData.first;
        float riverMask = riverData.second;
        float riverDepth = isRiver && baseHeight < 100
                               ? glm::smoothstep(0.92f, 1.0f, riverMask)
                               : 0.0f;

        std::pair<bool, float> holeData = m_WorldGen->GenHoles(worldX, worldZ);
        bool isHole = holeData.first;
        float holeMask = holeData.second;
        float holeDepth =
            isHole ? glm::smoothstep(0.92f, 1.0f, holeMask) : 0.0f;

        std::pair<bool, float> oceanData =
            m_WorldGen->GenOceans(worldX, worldZ);
        bool isOcean = oceanData.first;
        float oceanMask = oceanData.second;
        float oceanDepth = isOcean && baseHeight < 100
                               ? glm::smoothstep(0.92f, 1.0f, oceanMask)
                               : 0.0f;

        int carvedHeight = baseHeight - static_cast<int>(riverDepth * 16) -
                           static_cast<int>(holeDepth * 40) -
                           static_cast<int>(oceanMask * 50);

        height = carvedHeight;
    }

    GetCam3D().position.x = static_cast<float>(worldX) * 0.2f;
    GetCam3D().position.z = static_cast<float>(worldZ) * 0.2f;
    GetCam3D().position.y = (static_cast<float>(height) + 5) * 0.2f;
}

void Game::m_UpdateControls() {
    float aspect = GetScreenWidth() / GetScreenHeight();
    Camera3D &cam = GetCam3D();
    cam.UpdateFrustum(aspect);

    if (m_F3Mode) {
        if (IsKeyPressedOnce(KEY_U) && m_UseLighting)
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

        if (IsKeyPressedOnce(KEY_L)) {
            m_UseLighting = !m_UseLighting;
            m_UseShadows = m_UseLighting;
        }
    }

    if (IsKeyPressedOnce(KEY_F2))
        Screenshot::TakeScreenshot(
            "build/", glm::ivec2(GetScreenWidth(), GetScreenHeight()));

    if (IsKeyPressedOnce(KEY_F3))
        m_F3Mode = !m_F3Mode;

    EnableMSAA(m_UseMSAA);

    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        m_PlayerVel.y -= 1 * GetDeltaTime();
    }
    if (IsKeyDown(KEY_SPACE) && m_PlayerGround) {
        m_PlayerVel.y = 0.12f;
        m_PlayerGround = false;
        m_PressedKey = true;
    }

    glm::vec3 wishDir(0.0f);

    if (IsKeyDown(KEY_W))
        wishDir += cam.front;
    if (IsKeyDown(KEY_S))
        wishDir -= cam.front;
    if (IsKeyDown(KEY_D))
        wishDir += glm::normalize(glm::cross(cam.front, cam.up));
    if (IsKeyDown(KEY_A))
        wishDir -= glm::normalize(glm::cross(cam.front, cam.up));

    if (glm::length(wishDir) > 0.0f)
        wishDir = glm::normalize(wishDir);

    float accel = 0.5f;
    float maxSpeed = 0.03f;
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        accel *= 2.0f;
        maxSpeed *= 3;
    }

    m_PlayerVel.x += wishDir.x * accel * GetDeltaTime();
    m_PlayerVel.z += wishDir.z * accel * GetDeltaTime();

    if (m_PlayerGround) {
        float friction = 12.0f * GetDeltaTime();
        m_PlayerVel.x -= m_PlayerVel.x * friction;
        m_PlayerVel.z -= m_PlayerVel.z * friction;
    }

    m_PlayerVel.x = glm::clamp(m_PlayerVel.x, -maxSpeed, maxSpeed);
    m_PlayerVel.z = glm::clamp(m_PlayerVel.z, -maxSpeed, maxSpeed);

    if (IsKeyPressedOnce(KEY_ESCAPE))
        DestroyWindow();
}

void Game::m_ResolveAxis(const int axis) {
    constexpr float bs = 0.2f;
    constexpr float pw = 0.15f;
    constexpr float ph = 0.35f;

    Camera3D &cam = GetCam3D();

    glm::vec3 min(cam.position.x - (pw * 0.5f), cam.position.y - (ph * 0.75f),
                  cam.position.z - (pw * 0.5f));

    glm::vec3 max(cam.position.x + (pw * 0.5f), cam.position.y + (ph * 0.25f),
                  cam.position.z + (pw * 0.5f));

    glm::ivec3 bmin = glm::floor(min / bs);
    glm::ivec3 bmax = glm::floor(max / bs);

    for (int x = bmin.x; x <= bmax.x; x++)
        for (int y = bmin.y; y <= bmax.y; y++)
            for (int z = bmin.z; z <= bmax.z; z++) {

                Block block = m_WorldGen->manager.GetBlockGlobal({x, y, z});
                if (!block.IsSolid() || block.IsFluid())
                    continue;

                glm::vec3 blockMin = glm::vec3(x, y, z) * bs;
                glm::vec3 blockMax = blockMin + glm::vec3(bs);

                if (min.x >= blockMax.x || max.x <= blockMin.x ||
                    min.y >= blockMax.y || max.y <= blockMin.y ||
                    min.z >= blockMax.z || max.z <= blockMin.z)
                    continue;

                if (axis == 0)
                    m_ResolveX(blockMin, blockMax);
                else if (axis == 1)
                    m_ResolveY(blockMin, blockMax);
                else
                    m_ResolveZ(blockMin, blockMax);

                return;
            }
}

void Game::m_ResolveX(const glm::vec3 &min, const glm::vec3 &max) {
    constexpr float pw = 0.15f;
    auto &cam = GetCam3D();

    if (m_PlayerVel.x > 0)
        cam.position.x = min.x - (pw * 0.5f);
    else
        cam.position.x = max.x + (pw * 0.5f);

    m_PlayerVel.x = 0;
}

void Game::m_ResolveY(const glm::vec3 &min, const glm::vec3 &max) {
    constexpr float ph = 0.35f;
    auto &cam = GetCam3D();

    if (m_PlayerVel.y > 0) {
        cam.position.y = min.y - (ph * 0.25f);
    } else {
        cam.position.y = max.y + (ph * 0.75f);
        m_PlayerGround = true;
    }
    m_PlayerVel.y = 0;
}

void Game::m_ResolveZ(const glm::vec3 &min, const glm::vec3 &max) {
    constexpr float pw = 0.15f;
    auto &cam = GetCam3D();

    if (m_PlayerVel.z > 0)
        cam.position.z = min.z - (pw * 0.5f);
    else
        cam.position.z = max.z + (pw * 0.5f);

    m_PlayerVel.z = 0;
}

void Game::m_MoveAndCollide() {
    m_PlayerGround = false;

    Camera3D &cam = GetCam3D();

    int x = 0;
    int y = 1;
    int z = 2;
    cam.position.x += m_PlayerVel.x;
    m_ResolveAxis(x);

    cam.position.y += m_PlayerVel.y;
    m_ResolveAxis(y);

    cam.position.z += m_PlayerVel.z;
    m_ResolveAxis(z);
}

void Game::m_UpdatePhysics() {
    if (m_PlayerVel.y > -1)
        m_PlayerVel.y -= 1 * GetDeltaTime();
}

void Game::m_Update() {
    if (m_WorldGen->manager.chunks.find(ChunkManager::GetPlayerChunkPos(
            GetCam3D().position)) != m_WorldGen->manager.chunks.end() &&
        m_WorldGen->manager.chunks
                .at(ChunkManager::GetPlayerChunkPos(GetCam3D().position))
                .state == Chunk::MeshState::READY)
        m_UpdatePhysics();

    m_UpdateControls();

    m_MoveAndCollide();

    m_WorldGen->manager.ProcessFinishedChunks();
    m_WorldGen->manager.ProcessDirtyChunks(m_ViewDist, 1);
    m_WorldGen->manager.UploadChunkMeshes();

    m_WorldGen->UpdateMap();

    m_WorldGen->manager.UpdateVisibleChunksDepth(m_ViewDist);
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

void Game::m_ComputeShadows(glm::vec3 &lightDir,
                            glm::mat4 &lightSpaceMatrix) const {
    constexpr float shadowRange = 40.0f;

    lightDir =
        glm::normalize(glm::vec3(std::sin(GetTime() * 0.1f) * 3.3f, -2.0f,
                                 std::cos(GetTime() * 0.1f) * 3.3f));
    glm::vec3 center =
        GetCam3D().position + GetCam3D().front * (shadowRange * 0.5f);
    glm::vec3 lightPos = center - lightDir * shadowRange;
    glm::mat4 lightProjection =
        glm::ortho(-shadowRange, shadowRange, -shadowRange, shadowRange,
                   -shadowRange * 2.0f, shadowRange * 2.0f);
    glm::mat4 lightView =
        glm::lookAt(lightPos, center, glm::vec3(0.0f, 1.0f, 0.0f));
    float texelSize = (shadowRange * 2.0f) / static_cast<float>(m_ShadowRes);
    glm::vec4 centerLS = lightView * glm::vec4(center, 1.0f);
    centerLS.x = std::floor(centerLS.x / texelSize) * texelSize;
    centerLS.y = std::floor(centerLS.y / texelSize) * texelSize;
    glm::vec3 snappedCenter = glm::vec3(glm::inverse(lightView) * centerLS);
    lightView =
        glm::lookAt(lightPos, snappedCenter, glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
}

uint32_t Game::m_DrawOpaque(const glm::vec3 &lightDir,
                            const glm::mat4 &lightSpaceMatrix) {
    EnableFaceCulling(true);

    BeginDraw(DrawModes::CUBE_TEX);

    DrawCube(GetCam3D().position - lightDir * 500.0f, glm::vec3(25), WHITE);

    if (m_UseLighting)
        BeginDraw(DrawModes::CUBE_TEX_LIGHT);

    SetShininess3D(1);

    DirectionalLight day(glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)),
                         Color(76.5, 76.5, 76.5, 255), glm::vec3(1.0f),
                         glm::vec3(1.0f));

    DirectionalLight night(glm::normalize(glm::vec3(-0.2f, 1.0f, -0.3f)),
                           Color(0, 6, 83, 255), glm::vec3(0.5f),
                           glm::vec3(0.0f));

    SetDirLight3D(day);

    if (m_UseShadows) {
        Shader &shader = GetShader(m_UseLighting ? DrawModes::CUBE_TEX_LIGHT
                                                 : DrawModes::CUBE_TEX);
        shader.SetMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);
        shader.SetInt("shadowMap", 1);
        m_ShadowMap->BindForReading(1);
    }

    m_WorldGen->manager.UpdateVisibleChunks(m_ViewDist);

    uint32_t chunksDrawn = m_WorldGen->manager.DrawChunks(
        GetShader(m_UseLighting ? DrawModes::CUBE_TEX_LIGHT
                                : DrawModes::CUBE_TEX),
        m_DepthShader, m_TexAtlases);

    return chunksDrawn;
}

void Game::m_DrawTransparent() {
    BeginDraw(DrawModes::CUBE_TEX);

    EnableTransparency();

    EnableDepth(false);
    EnableFaceCulling(false);
    glDisable(GL_CULL_FACE);
    for (auto &cloud : m_Clouds) {
        DrawCube(cloud.pos, cloud.size, Color(255, 255, 255, 100));
    }

    BeginDraw(DrawModes::CUBE_TEX_LIGHT);

    SetShininess3D(128);
    m_WorldGen->manager.DrawTransparentChunks(
        GetShader(m_UseLighting ? DrawModes::CUBE_TEX_LIGHT
                                : DrawModes::CUBE_TEX),
        m_TexAtlases);
    EnableDepth(true);
}

void Game::m_DrawUI(const uint32_t chunksDrawn) {
    BeginDraw(DrawModes::TEX, false);

    Texture2D *logoTex = TextureLoader::GetCPLLogo();
    DrawTex2D(logoTex,
              {GetScreenWidth() - logoTex->size.x,
               GetScreenHeight() - logoTex->size.y},
              WHITE);

    BeginDraw(DrawModes::TEXT, false);
    // TODO Font kinda sux idk
    // Text::Use("minecraftFont");

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

    int textPos = 0;

    DrawTextShadow(
        {0, textPos}, {3, -3}, 0.5f,
        "X: " + std::to_string(static_cast<int>(GetCam3D().position.x * 5)) +
            ", Y: " +
            std::to_string(static_cast<int>(GetCam3D().position.y * 5)) +
            ", Z: " +
            std::to_string(static_cast<int>(GetCam3D().position.z * 5)),
        WHITE, DARK_GRAY);

    textPos += 30;

    DrawTextShadow({0, textPos}, {3, -3}, 0.5f,
                   "Seed: " + std::to_string(m_WorldGen->seed), WHITE,
                   DARK_GRAY);

    textPos += 60;

    DrawTextShadow({0, textPos}, {3, -3}, 0.5f, "--- Chunks ---", WHITE,
                   DARK_GRAY);

    textPos += 30;

    DrawTextShadow({0, textPos}, {3, -3}, 0.5f,
                   "Chunks - Ready: " + std::to_string(ready) +
                       " | Local: " + std::to_string(meshLocal) +
                       " | Dirty: " + std::to_string(dirty) +
                       " | None: " + std::to_string(none),
                   WHITE, DARK_GRAY);

    textPos += 30;

    DrawTextShadow({0, textPos}, {3, -3}, 0.5f,
                   "Chunks drawn: " + std::to_string(chunksDrawn) + " / " +
                       std::to_string(m_WorldGen->manager.chunks.size()),
                   WHITE, DARK_GRAY);

    textPos += 60;

    DrawTextShadow({0, textPos}, {3, -3}, 0.5f, "--- Graphics ---", WHITE,
                   DARK_GRAY);

    textPos += 30;

    DrawTextShadow({0, textPos}, {3, -3}, 0.5f,
                   "Lighting: " + std::string(m_UseLighting ? "ON" : "OFF"),
                   WHITE, DARK_GRAY);

    textPos += 30;

    DrawTextShadow({0, textPos}, {3, -3}, 0.5f,
                   "Shadows: " + std::string(m_UseShadows ? "ON" : "OFF"),
                   WHITE, DARK_GRAY);

    textPos += 30;

    DrawTextShadow({0, textPos}, {3, -3}, 0.5f,
                   "Shadow resolution: " + std::to_string(m_ShadowRes), WHITE,
                   DARK_GRAY);

    textPos += 30;

    DrawTextShadow({0, textPos}, {3, -3}, 0.5f,
                   "Anti Aliasing: " + std::string(m_UseMSAA ? "MSAA" : "NONE"),
                   WHITE, DARK_GRAY);

    textPos += 60;

    DrawTextShadow(
        {0, textPos}, {3, -3}, 0.5f,
        "Stack used: " +
            std::to_string(static_cast<float>(Profiler::GetStackUsed()) /
                           static_cast<float>(Profiler::GetStackSize())) +
            "% (" +
            std::to_string(static_cast<float>(Profiler::GetStackUsed()) /
                           1000000.0f) +
            " / " +
            std::to_string(static_cast<float>(Profiler::GetStackSize()) /
                           1000000.0f) +
            " MB)",
        WHITE, DARK_GRAY);

    textPos += 30;

    DrawTextShadow(
        {0, textPos}, {3, -3}, 0.5f,
        "Heap used: " +
            std::to_string(static_cast<float>(Profiler::GetHeapUsed()) /
                           1000000.0f) +
            " MB",
        WHITE, DARK_GRAY);

    DrawTextShadow({GetScreenWidth() / 2, 0}, {7, -7}, 1.0f,
                   GetCardinalDir(GetCam3D().yaw), WHITE, DARK_GRAY);

    ShowDetails();
}

void Game::m_Draw() {
    glm::mat4 lightSpaceMatrix(1);
    glm::vec3 lightDir;
    if (m_UseShadows) {
        m_ComputeShadows(lightDir, lightSpaceMatrix);
    }
    m_WorldGen->manager.DrawShadowMapChunks(m_ShadowMap.get(), lightSpaceMatrix,
                                            m_TexAtlases, m_UseShadows);

    ClearBackground(SKY_BLUE);

    float skyRot = GetTime() * 0.5f;
    DrawCubeMapRot(m_Skybox.get(), glm::vec3(0, skyRot, 0));

    uint32_t chunksDrawn = m_DrawOpaque(lightDir, lightSpaceMatrix);

    m_DrawTransparent();

    if (m_F3Mode)
        m_DrawUI(chunksDrawn);

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
