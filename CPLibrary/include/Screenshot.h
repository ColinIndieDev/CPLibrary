#pragma once

#include <glm/glm.hpp>
#include <string>

namespace Screenshot {
inline int screenshotsTaken = 0;
void TakeScreenshot(const std::string &folderPath,
                    const glm::ivec2 &screenSize);
} // namespace Screenshot
