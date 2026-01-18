#pragma once

#include "../CPLibrary/CPLibrary.h"
#include "Block.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "FastNoiseLite.h"
#include "TextureLoader.h"
#include "WorldGen.h"
#include <memory>

using namespace CPL;

struct Cloud {
    glm::vec3 pos;
    glm::vec3 size;
    Cloud(const glm::vec3 &pos, const glm::vec3 &size) : pos(pos), size(size) {}
};

struct Player {
    glm::vec3 pos = glm::vec3(0);
    glm::vec3 size = glm::vec3(0.15f, 0.37f, 0.15f);
    glm::vec3 vel = glm::vec3(0);
    glm::vec3 raycastBlock = glm::vec3(0);
    bool hitBlock = false;
    bool ground = false;

    std::unordered_map<int, BlockType> creativeInventory = {
        {0, BlockType::GRASS_BLOCK}, {1, BlockType::DIRT},
        {2, BlockType::SAND},        {3, BlockType::STONE},
        {4, BlockType::SNOW},        {5, BlockType::BEDROCK},
        {6, BlockType::OAK_LOG},     {7, BlockType::OAK_LEAVES},
    };
    int selectedBlock = 0;
};

struct Raycast {
    bool hit = false;
    glm::ivec3 block;
    glm::ivec3 normal;
};

class Game {
  public:
    void Init();
    void Run();

  private:
    std::unique_ptr<WorldGen> m_WorldGen;
    std::unique_ptr<CubeMap> m_Skybox;
    std::unique_ptr<ShadowMap> m_ShadowMap;
    std::map<BlockType, Texture2D *> m_TexAtlases;
    int m_ViewDist = 16; // Default: 16 (chunks)

    uint32_t m_ShadowRes = 4096;
    bool m_UseShadows = true;
    bool m_UseLighting = true;

    bool m_UseMSAA = true;

    // Temporarily
    std::vector<Cloud> m_Clouds;

    Player m_Player;

    bool m_PressedKey = false;
    bool m_F3Mode = false;

    Shader m_DepthShader;

    void m_UpdateMovementCtrl();
    void m_UpdateRaycastCtrl();
    void m_UpdateConfigCtrl();
    void m_UpdateControls();
    Raycast m_RaycastBlock(glm::vec3 origin, glm::vec3 dir, float maxDist);
    void m_UpdatePhysics();
    void m_MoveAndCollide();
    void m_ResolveAxis(int axis);
    void m_ResolveX(const glm::vec3 &min, const glm::vec3 &max);
    void m_ResolveY(const glm::vec3 &min, const glm::vec3 &max);
    void m_ResolveZ(const glm::vec3 &min, const glm::vec3 &max);
    void m_Update();
    void m_ComputeShadows(glm::vec3 &lightDir,
                          glm::mat4 &lightSpaceMatrix) const;
    uint32_t m_DrawOpaque(const glm::vec3 &lightDir,
                          const glm::mat4 &lightSpaceMatrix);
    void m_DrawTransparent();
    void m_DrawUI(uint32_t chunksDrawn);
    void m_Draw();
    void m_SetSpawnPoint();
    void m_InitAtlases();
};
