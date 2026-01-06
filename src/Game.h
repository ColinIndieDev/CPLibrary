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

class Game {
  public:
    void Init();
    void Run();

  private:
    std::unique_ptr<WorldGen> m_WorldGen;
    std::unique_ptr<CubeMap> m_Skybox;
    std::map<BlockType, Texture2D *> m_TexAtlases;
    
    static void m_Update();
    void m_Draw();
    void m_SetSpawnPoint();
    void m_InitAtlases();
};
