#pragma once
#include "../CPL.h"

namespace CPL {
class Shader;
class CubeMap {
  public:
    explicit CubeMap(const std::string &path);
    static uint32_t
    LoadCubeMapFromImages(const std::vector<std::string> &faces);
    static std::vector<unsigned char>
    ExtractSubImage(const unsigned char *fullImage, int fullWidth,
                    int fullHeight, int xOff, int yOff, int faceWidth,
                    int faceHeight, int channels);
    static uint32_t LoadCubeMapFromCross(const std::string &path);
    void Draw(const Shader &shader) const;

  private:
    uint32_t m_VAO{}, m_VBO{}, m_CubeMapTex;
};
} // namespace CPL
