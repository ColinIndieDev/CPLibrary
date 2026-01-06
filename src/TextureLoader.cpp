#include "TextureLoader.h"

std::vector<std::unique_ptr<Texture2D>> TextureLoader::s_BlockTexs(9);

std::unique_ptr<Texture2D> TextureLoader::s_CPLLogo;

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
        std::make_unique<Texture2D>("assets/images/snow.png",
                                    glm::vec2(48, 32),
                                    TextureFiltering::NEAREST);

    s_BlockTexs[static_cast<int>(BlockType::GRASS)] =
        std::make_unique<Texture2D>("assets/images/grass.png", glm::vec2(16),
                                    TextureFiltering::NEAREST);
    s_BlockTexs[static_cast<int>(BlockType::RED_TULP)] =
        std::make_unique<Texture2D>("assets/images/red_tulp.png", glm::vec2(16),
                                    TextureFiltering::NEAREST);

    s_CPLLogo = std::make_unique<Texture2D>(
        "assets/images/logo.png", glm::vec2(200), TextureFiltering::LINEAR);
}

Texture2D *TextureLoader::GetBlockTex(const BlockType &type) {
    return s_BlockTexs[static_cast<int>(type)].get();
}

Texture2D *TextureLoader::GetCPLLogo() { return s_CPLLogo.get(); }
