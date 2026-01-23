#include "Game.h"
#include "Block.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "TextureLoader.h"
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

    m_DepthShader = Shader("assets/shaders/default/vert/3D/lightShape.vert",
                           "assets/shaders/default/frag/3D/zPrepass.frag");

    uint32_t seed = RandInt(0, 999999999);

    m_WorldGen =
        std::make_unique<WorldGen>(seed, m_ViewDist, 30, Chunk::s_Height, 60);
    m_WorldGen->Init();
    m_SetSpawnPoint();
    m_WorldGen->GenMap();

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

    m_Player.pos.x = static_cast<float>(worldX) * 0.2f;
    m_Player.pos.z = static_cast<float>(worldZ) * 0.2f;
    m_Player.pos.y = (static_cast<float>(height) + 2) * 0.2f;
    GetCam3D().position =
        glm::vec3(m_Player.pos.x, m_Player.pos.y + (m_Player.size.y * 0.45f),
                  m_Player.pos.z);
}

inline int PositiveMod(const int x, const int y) {
    const int result = x % y;
    return result < 0 ? result + y : result;
}

// {{{ Controls
void Game::m_UpdateConfigCtrl() {
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

        if (IsKeyPressedOnce(KEY_F))
            m_UseZPrePass = !m_UseZPrePass;

        if (IsKeyPressedOnce(KEY_G)) {
            m_ViewDist *= 2;
            m_WorldGen->viewDist = m_ViewDist;
            m_FogStart = 2.81f * static_cast<float>(m_ViewDist);
            m_FogEnd = m_FogStart - 6.0f;
        }
        if (IsKeyPressedOnce(KEY_B)) {
            m_ViewDist /= 2;
            m_WorldGen->viewDist = m_ViewDist;
            m_FogStart = 2.81f * static_cast<float>(m_ViewDist);
            m_FogEnd = m_FogStart - 6.0f;
        }
    }

    if (IsKeyPressedOnce(KEY_F2))
        Screenshot::TakeScreenshot(
            "build/", glm::ivec2(GetScreenWidth(), GetScreenHeight()));

    if (IsKeyPressedOnce(KEY_F3))
        m_F3Mode = !m_F3Mode;

    if (IsKeyPressedOnce(KEY_C)) {
        m_Player.selectedBlock =
            (m_Player.selectedBlock + 1) %
            static_cast<int>(m_Player.creativeInventory.size());
    } else if (IsKeyPressedOnce(KEY_X)) {
        if (m_Player.selectedBlock - 1 < 0)
            m_Player.selectedBlock =
                static_cast<int>(m_Player.creativeInventory.size()) - 1;
        else
            m_Player.selectedBlock--;
    }

    EnableMSAA(m_UseMSAA);
}

void Game::m_UpdateMovementCtrl() {
    float aspect = GetScreenWidth() / GetScreenHeight();
    Camera3D &cam = GetCam3D();
    cam.UpdateFrustum(aspect);
    cam.position =
        glm::vec3(m_Player.pos.x, m_Player.pos.y + (m_Player.size.y * 0.45f),
                  m_Player.pos.z);

    if (IsKeyDown(KEY_SPACE) && m_Player.ground) {
        m_Player.vel.y = 0.09f;
        m_Player.ground = false;
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

    m_Player.vel.x += wishDir.x * accel * GetDeltaTime();
    m_Player.vel.z += wishDir.z * accel * GetDeltaTime();

    if (m_Player.ground) {
        float friction = 12.0f * GetDeltaTime();
        m_Player.vel.x -= m_Player.vel.x * friction;
        m_Player.vel.z -= m_Player.vel.z * friction;
    } else {
        float friction = 6.0f * GetDeltaTime();
        m_Player.vel.x -= m_Player.vel.x * friction;
        m_Player.vel.z -= m_Player.vel.z * friction;
    }

    m_Player.vel.x = glm::clamp(m_Player.vel.x, -maxSpeed, maxSpeed);
    m_Player.vel.z = glm::clamp(m_Player.vel.z, -maxSpeed, maxSpeed);
}

void Game::m_UpdateRaycastCtrl() {
    float aspect = GetScreenWidth() / GetScreenHeight();
    Camera3D &cam = GetCam3D();

    auto hit = m_RaycastBlock(cam.position, cam.front, 1.0f);
    m_Player.hitBlock = hit.hit;
    if (hit.hit) {
        m_Player.raycastBlock = glm::vec3(hit.block) * 0.2f;
        if (IsMousePressedOnce(MOUSE_BUTTON_LEFT)) {
            // Debug
            m_Rays.emplace_back(cam.position, cam.position + cam.front * 10.0f);

            if (!m_WorldGen->manager.GetBlockGlobal(hit.block).IsUnbreakable())
                m_WorldGen->manager.DestroyBlock(hit.block);
        }
        if (IsMousePressedOnce(MOUSE_BUTTON_RIGHT)) {
            glm::ivec3 placedBlockPos(hit.block + hit.normal);

            constexpr float bs = 0.2f;
            Cube placedBlockCollider(glm::vec3(placedBlockPos) * 0.2f,
                                     glm::vec3(bs), WHITE);
            Cube playerCollider(m_Player.pos, m_Player.size, WHITE);

            if (!CheckCollisionCubes(playerCollider, placedBlockCollider)) {
                BlockType selectedType =
                    m_Player.creativeInventory[m_Player.selectedBlock];
                m_WorldGen->manager.PlaceBlock(hit.block + hit.normal,
                                               selectedType);
            }
        }
    }
}

void Game::m_UpdateControls() {
    m_UpdateConfigCtrl();
    m_UpdateMovementCtrl();
    m_UpdateRaycastCtrl();

    if (IsKeyPressedOnce(KEY_ESCAPE))
        DestroyWindow();
}

Raycast Game::m_RaycastBlock(glm::vec3 origin, glm::vec3 dir,
                             const float maxDist) {
    constexpr float bs = 0.2f;

    origin = origin / bs + glm::vec3(0.5f);

    dir = glm::normalize(dir);

    glm::ivec3 block = glm::floor(origin);

    glm::vec3 deltaDist = glm::abs(glm::vec3(1.0f) / dir);

    glm::ivec3 step;
    glm::vec3 sideDist;

    for (int i = 0; i < 3; i++) {
        if (dir[i] < 0) {
            step[i] = -1;
            sideDist[i] =
                (origin[i] - static_cast<float>(block[i])) * deltaDist[i];
        } else {
            step[i] = 1;
            sideDist[i] = (static_cast<float>(block[i]) + 1.0f - origin[i]) *
                          deltaDist[i];
        }
    }

    float traveled = 0.0f;
    glm::ivec3 hitNormal(0);

    while (traveled < maxDist / bs) {
        if (sideDist.x < sideDist.y && sideDist.x < sideDist.z) {
            traveled = sideDist.x;
            sideDist.x += deltaDist.x;
            block.x += step.x;
            hitNormal = {-step.x, 0, 0};
        } else if (sideDist.y < sideDist.z) {
            traveled = sideDist.y;
            sideDist.y += deltaDist.y;
            block.y += step.y;
            hitNormal = {0, -step.y, 0};
        } else {
            traveled = sideDist.z;
            sideDist.z += deltaDist.z;
            block.z += step.z;
            hitNormal = {0, 0, -step.z};
        }

        Block b = m_WorldGen->manager.GetBlockGlobal(block);
        if (b.IsSolid() && !b.IsFluid()) {
            return {true, block, hitNormal};
        }
    }

    return {};
}
// }}}

// {{{ Collisions
void Game::m_ResolveAxis(const int axis) {
    constexpr float bs = 0.2f;
    const float pw = m_Player.size.x;
    const float ph = m_Player.size.y;

    glm::vec3 min(m_Player.pos.x - (pw * 0.5f), m_Player.pos.y - (ph * 0.5f),
                  m_Player.pos.z - (pw * 0.5f));

    glm::vec3 max(m_Player.pos.x + (pw * 0.5f), m_Player.pos.y + (ph * 0.5f),
                  m_Player.pos.z + (pw * 0.5f));

    glm::ivec3 bmin = glm::floor((min - bs * 0.5f) / bs);
    glm::ivec3 bmax = glm::floor((max + bs * 0.5f) / bs);

    for (int x = bmin.x; x <= bmax.x; x++)
        for (int y = bmin.y; y <= bmax.y; y++)
            for (int z = bmin.z; z <= bmax.z; z++) {

                Block block = m_WorldGen->manager.GetBlockGlobal({x, y, z});
                if (!block.IsSolid() || block.IsFluid())
                    continue;

                glm::vec3 center = glm::vec3(x, y, z) * bs;
                glm::vec3 half(bs * 0.5f);

                glm::vec3 blockMin = center - half;
                glm::vec3 blockMax = center + half;

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
    const float pw = m_Player.size.x;

    if (m_Player.vel.x > 0)
        m_Player.pos.x = min.x - (pw * 0.5f);
    else
        m_Player.pos.x = max.x + (pw * 0.5f);

    m_Player.vel.x = 0;
}

void Game::m_ResolveY(const glm::vec3 &min, const glm::vec3 &max) {
    const float ph = m_Player.size.y;

    if (m_Player.vel.y > 0) {
        m_Player.pos.y = min.y - (ph * 0.5f);
    } else {
        m_Player.pos.y = max.y + (ph * 0.5f);
        m_Player.ground = true;
    }
    m_Player.vel.y = 0;
}

void Game::m_ResolveZ(const glm::vec3 &min, const glm::vec3 &max) {
    const float pw = m_Player.size.z;

    if (m_Player.vel.z > 0)
        m_Player.pos.z = min.z - (pw * 0.5f);
    else
        m_Player.pos.z = max.z + (pw * 0.5f);

    m_Player.vel.z = 0;
}

void Game::m_MoveAndCollide() {
    m_Player.ground = false;

    int x = 0;
    int y = 1;
    int z = 2;
    m_Player.pos.x += m_Player.vel.x;
    m_ResolveAxis(x);

    m_Player.pos.y += m_Player.vel.y;
    m_ResolveAxis(y);

    m_Player.pos.z += m_Player.vel.z;
    m_ResolveAxis(z);
}

void Game::m_UpdatePhysics() {
    if (m_Player.vel.y > -1)
        m_Player.vel.y -= 1 * GetDeltaTime();
}
// }}}

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

// {{{ Drawing
void Game::m_ComputeShadows(glm::vec3 &lightDir,
                            glm::mat4 &lightSpaceMatrix) const {
    constexpr float shadowRange = 40.0f;

    lightDir =
        glm::normalize(glm::vec3(std::sin(GetTime() * 0.1f) * 3.0f, -1.7f,
                                 std::cos(GetTime() * 0.1f) * 3.0f));
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
    BeginDraw(DrawModes::SHAPE_3D);

    glLineWidth(5.0f);
    for (auto &ray : m_Rays) {
        DrawRay(ray.first, ray.second, RED);
    }
    glLineWidth(1.0f);

    EnableFaceCulling(true);

    BeginDraw(DrawModes::SHAPE_3D);

    EnableFog(false);
    DrawCube(GetCam3D().position - lightDir * 500.0f, glm::vec3(25), YELLOW);

    if (m_UseLighting)
        BeginDraw(DrawModes::SHAPE_3D_LIGHT);

    SetShininess3D(1);

    DirectionalLight day(glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)),
                         Color(76.5, 76.5, 76.5, 255), glm::vec3(1.0f),
                         glm::vec3(1.0f));

    DirectionalLight night(glm::normalize(glm::vec3(-0.2f, 1.0f, -0.3f)),
                           Color(0, 6, 83, 255), glm::vec3(0.5f),
                           glm::vec3(0.0f));

    SetDirLight3D(day);

    if (m_UseShadows) {
        Shader &shader = GetShader(m_UseLighting ? DrawModes::SHAPE_3D_LIGHT
                                                 : DrawModes::SHAPE_3D);
        shader.SetMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);
        shader.SetInt("shadowMap", 1);
        m_ShadowMap->BindForReading(1);
    }

    EnableFog(true);
    SetFog(m_FogStart, m_FogEnd, Color(133, 174, 255, 255));

    m_WorldGen->manager.UpdateVisibleChunks(m_ViewDist);

    uint32_t chunksDrawn = m_WorldGen->manager.DrawChunks(
        GetShader(m_UseLighting ? DrawModes::SHAPE_3D_LIGHT
                                : DrawModes::SHAPE_3D),
        m_DepthShader, m_TexAtlases, m_UseZPrePass);

    return chunksDrawn;
}

void Game::m_DrawTransparent() {
    BeginDraw(DrawModes::SHAPE_3D);

    EnableFog(true);
    SetFog(m_FogStart * 2, m_FogEnd * 2, Color(133, 174, 255, 255));
    GetShader(DrawModes::SHAPE_3D).SetVector3f("viewPos", GetCam3D().position);

    EnableTransparency();
    EnableDepth(false);

    if (m_Player.hitBlock) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(5.0f);

        DrawCube(m_Player.raycastBlock, glm::vec3(0.201f),
                 Color(255, 255, 255, 255));

        glLineWidth(1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    std::sort(m_Clouds.begin(), m_Clouds.end(), [&](Cloud &a, Cloud &b) {
        glm::vec3 aPos = a.pos;
        glm::vec3 bPos = b.pos;

        float da = glm::length2(aPos - GetCam3D().position);
        float db = glm::length2(bPos - GetCam3D().position);

        return da < db;
    });

    for (auto &cloud : m_Clouds) {
        DrawCube(cloud.pos, cloud.size, Color(255, 255, 255, 200));
    }

    BeginDraw(DrawModes::SHAPE_3D_LIGHT);

    SetFog(m_FogStart, m_FogEnd, Color(133, 174, 255, 255));

    EnableFaceCulling(false);
    SetShininess3D(128);
    m_WorldGen->manager.DrawTransparentChunks(
        GetShader(m_UseLighting ? DrawModes::SHAPE_3D_LIGHT
                                : DrawModes::SHAPE_3D),
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
        std::lock_guard<std::mutex> lock(m_WorldGen->manager.chunksMutex);
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
                   "Render distance: " + std::to_string(m_ViewDist) + " chunks",
                   WHITE, DARK_GRAY);

    textPos += 30;

    DrawTextShadow({0, textPos}, {3, -3}, 0.5f,
                   "Z Prepass: " + std::string(m_UseZPrePass ? "ON" : "OFF"),
                   WHITE, DARK_GRAY);

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

    BeginDraw(DrawModes::SHAPE_2D, false);

    glm::vec2 screenCenter(GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f);
    glm::vec2 crossHairSize(10.0f);
    DrawRect(screenCenter - glm::vec2(crossHairSize * 0.5f), crossHairSize,
             WHITE);

    BeginDraw(DrawModes::TEX, false);

    Texture2D *hotbar = TextureLoader::GetHotbar();
    float hotbarOff = 5.0f;
    glm::vec2 hotbarPos((GetScreenWidth() * 0.5f) - (hotbar->size.x * 0.5f),
                        GetScreenHeight() - hotbar->size.y - hotbarOff);
    DrawTex2D(hotbar, hotbarPos, WHITE);

    Texture2D *hotbarHighlight = TextureLoader::GetHotbarHighlight();
    float hotbarHighlightOff = 2.5f;
    glm::vec2 hotbarHighlightPos(
        hotbarPos +
        glm::vec2(static_cast<float>(m_Player.selectedBlock) *
                      (hotbar->size.x / 9.1f),
                  0) -
        glm::vec2(hotbarHighlightOff));
    DrawTex2D(hotbarHighlight, hotbarHighlightPos, WHITE);

    if (m_F3Mode)
        m_DrawUI(chunksDrawn);

    EndDraw();
}
// }}}

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
