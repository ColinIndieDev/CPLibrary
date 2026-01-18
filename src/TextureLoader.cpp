#include "TextureLoader.h"
#include <memory>

std::vector<std::unique_ptr<Texture2D>> TextureLoader::s_BlockTexs(11);

std::unique_ptr<Texture2D> TextureLoader::s_CPLLogo;
std::unique_ptr<Texture2D> TextureLoader::s_HotbarTex;
std::unique_ptr<Texture2D> TextureLoader::s_HotbarHighlightTex;

void TextureLoader::Init() {
    s_BlockTexs[static_cast<int>(BlockType::BEDROCK)] =
        std::make_unique<Texture2D>("assets/images/bedrock.png",
                                    glm::vec2(48, 32),
                                    TextureFiltering::NEAREST);
    s_BlockTexs[static_cast<int>(BlockType::GRASS_BLOCK)] =
        std::make_unique<Texture2D>("assets/images/grass_block.png",
                                    glm::vec2(48, 32),
                                    TextureFiltering::NEAREST);
    s_BlockTexs[static_cast<int>(BlockType::DIRT)] =
        std::make_unique<Texture2D>("assets/images/dirt.png", glm::vec2(48, 32),
                                    TextureFiltering::NEAREST);
    s_BlockTexs[static_cast<int>(BlockType::STONE)] =
        std::make_unique<Texture2D>("assets/images/stone.png",
                                    glm::vec2(48, 32),
                                    TextureFiltering::NEAREST);
    s_BlockTexs[static_cast<int>(BlockType::OAK_LOG)] =
        std::make_unique<Texture2D>("assets/images/oak_log.png",
                                    glm::vec2(48, 32),
                                    TextureFiltering::NEAREST);
    s_BlockTexs[static_cast<int>(BlockType::OAK_LEAVES)] =
        std::make_unique<Texture2D>("assets/images/oak_leaves.png",
                                    glm::vec2(48, 32),
                                    TextureFiltering::NEAREST);
    s_BlockTexs[static_cast<int>(BlockType::SNOW)] =
        std::make_unique<Texture2D>("assets/images/snow.png", glm::vec2(48, 32),
                                    TextureFiltering::NEAREST);
    s_BlockTexs[static_cast<int>(BlockType::SAND)] =
        std::make_unique<Texture2D>("assets/images/sand.png", glm::vec2(48, 32),
                                    TextureFiltering::NEAREST);
    s_BlockTexs[static_cast<int>(BlockType::WATER)] =
        std::make_unique<Texture2D>("assets/images/water.png",
                                    glm::vec2(48, 32),
                                    TextureFiltering::NEAREST);

    s_BlockTexs[static_cast<int>(BlockType::GRASS)] =
        std::make_unique<Texture2D>("assets/images/grass.png", glm::vec2(16),
                                    TextureFiltering::NEAREST);
    s_BlockTexs[static_cast<int>(BlockType::RED_TULP)] =
        std::make_unique<Texture2D>("assets/images/red_tulp.png", glm::vec2(16),
                                    TextureFiltering::NEAREST);

    s_CPLLogo = std::make_unique<Texture2D>("assets/images/default/logo.png",
                                            glm::vec2(300, 200),
                                            TextureFiltering::LINEAR);

    s_HotbarTex = std::make_unique<Texture2D>("assets/images/hotbar.png",
                                              glm::vec2(182 * 3, 22 * 3),
                                              TextureFiltering::NEAREST);

    s_HotbarHighlightTex = std::make_unique<Texture2D>(
        "assets/images/hotbar_highlight.png", glm::vec2(24 * 3),
        TextureFiltering::NEAREST);
}

Texture2D *TextureLoader::GetBlockTex(const BlockType &type) {
    return s_BlockTexs[static_cast<int>(type)].get();
}

Texture2D *TextureLoader::GetCPLLogo() { return s_CPLLogo.get(); }

Texture2D *TextureLoader::GetHotbar() { return s_HotbarTex.get(); }

Texture2D *TextureLoader::GetHotbarHighlight() {
    return s_HotbarHighlightTex.get();
}
