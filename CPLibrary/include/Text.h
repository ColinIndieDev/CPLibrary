#pragma once
#include "CPL.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <map>
#include <string>

namespace CPL {
struct Character {
    uint32_t textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t advance;

    Character(const uint32_t id, const glm::ivec2 &size,
              const glm::ivec2 &bearing, const uint32_t advance)
        : textureID(id), size(size), bearing(bearing), advance(advance) {}
};

class Text {
  public:
    static void Init(const std::string &fontPath, const std::string &fontName,
                     const TextureFiltering &textureFiltering);
    static void Use(const std::string &fontName);
    static void DrawText(const Shader &shader, const std::string &text,
                         glm::vec2 pos, float scale, const Color &color);
    static glm::vec2 GetTextSize(const std::string &fontName,
                                 const std::string &text, float scale);

  private:
    static std::map<std::string, std::map<GLchar, Character>> s_Fonts;
    static uint32_t s_VAO, s_VBO;
    static std::string s_CurFont;
};
} // namespace CPL
