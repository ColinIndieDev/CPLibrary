#pragma once
#include "../CPLibrary/CPLibrary.h"
#include <memory>

using namespace CPL;

class TextureLoader {
  public:
    enum class BlockType : uint8_t {
        BEDROCK,
        GRASS_BLOCK,
        DIRT,
        STONE,
        OAK_LOG,
        OAK_LEAVES,
        SNOW,
        SAND,
        WATER,
        GRASS,
        RED_TULP,
    };

    static void Init();
    static Texture2D* GetBlockTex(const BlockType &type);
    static Texture2D* GetCPLLogo();
    static Texture2D* GetHotbar();
    static Texture2D* GetHotbarHighlight();

  private:
    static std::vector<std::unique_ptr<Texture2D>> s_BlockTexs;
    static std::unique_ptr<Texture2D> s_CPLLogo;
    static std::unique_ptr<Texture2D> s_HotbarTex;
    static std::unique_ptr<Texture2D> s_HotbarHighlightTex;
};
