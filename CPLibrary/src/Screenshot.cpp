#include "../include/Screenshot.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "../include/Engine.h"

namespace Screenshot {

void TakeScreenshot(const std::string &folderPath, const glm::ivec2 &screenSize) {
    std::vector<uint8_t> pixels(
        static_cast<size_t>(screenSize.x * screenSize.y * 3));

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glReadPixels(0, 0, screenSize.x, screenSize.y, GL_RGB, GL_UNSIGNED_BYTE,
                 pixels.data());

    for (int y = 0; y < screenSize.y / 2; y++) {
        for (int x = 0; x < screenSize.x * 3; x++) {
            std::swap(pixels[(y * screenSize.x * 3) + x],
                      pixels[((screenSize.y - 1 - y) * screenSize.x * 3) + x]);
        }
    }

    std::string finalPath =
        folderPath + "screenshot" + std::to_string(screenshotsTaken) + ".png";
    stbi_write_png(finalPath.c_str(), screenSize.x, screenSize.y, 3,
                   pixels.data(), screenSize.x * 3);
    screenshotsTaken++;
}
} // namespace Screenshot
