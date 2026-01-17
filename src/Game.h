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

    glm::vec3 m_PlayerVel = glm::vec3(0);
    bool m_PlayerGround = false;
    bool m_PressedKey = false;
    bool m_F3Mode = false;

    Shader m_DepthShader;

    void m_UpdateControls();
    void m_UpdatePhysics();
    void m_MoveAndCollide();
    void m_ResolveAxis(int axis);
    void m_ResolveX(const glm::vec3 &min, const glm::vec3 &max);
    void m_ResolveY(const glm::vec3 &min, const glm::vec3 &max);
    void m_ResolveZ(const glm::vec3 &min, const glm::vec3 &max);
    void m_Update();
    void m_ComputeShadows(glm::vec3 &lightDir, glm::mat4 &lightSpaceMatrix) const;
    uint32_t m_DrawOpaque(const glm::vec3 &lightDir, const glm::mat4 &lightSpaceMatrix);
    void m_DrawTransparent();
    void m_DrawUI(uint32_t chunksDrawn);
    void m_Draw();
    void m_SetSpawnPoint();
    void m_InitAtlases();
};
