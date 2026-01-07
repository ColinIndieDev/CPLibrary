#pragma once

#include "../CPL.h"

namespace CPL {
class Shader;

class ScreenQuad {
  public:
    glm::vec2 size{};
    void Init(int width, int height);
    void BeginUseScreen() const;
    static void EndUseScreen();
    void Draw(int mode) const;
    void DrawCustom(const Shader &shader) const;

  private:
    uint32_t m_VBO{}, m_VAO{}, m_RBO{}, m_Framebuffer{}, m_TextureColorBuffer{};
};
} // namespace CPL
