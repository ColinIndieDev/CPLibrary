#pragma once
#include "../CPL.h"

namespace CPL {
class Shader;
class CubeMap {
  public:
    explicit CubeMap(const std::string &path);
    unsigned int LoadCubeMapFromImages(const std::vector<std::string> &faces);
    unsigned char *ExtractSubImage(unsigned char *fullImage, int fullWidth,
                                   int fullHeight, int xOffset, int yOffset,
                                   int faceWidth, int faceHeight, int channels);
    unsigned int LoadCubeMapFromCross(const std::string &path);
    void Draw(const Shader &shader);

  private:
    unsigned int VAO, VBO, cubeMapTexture;
};
} // namespace CPL
