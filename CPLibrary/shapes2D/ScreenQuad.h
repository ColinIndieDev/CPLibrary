#pragma once

#include "../CPL.h"

namespace CPL {
    class Shader;

    class ScreenQuad {
    public:
	glm::vec2 size{};
	void Init(int width, int height);
	void BeginUseScreen();
	void EndUseScreen();
	void Draw(int mode);
    private:
	unsigned int VBO{}, VAO{}, RBO{}, framebuffer{}, textureColorBuffer{}; 
    };
}
