#include "CubeMap.h"
#include "../Shader.h"
#include <stb_image.h>

namespace CPL {
CubeMap::CubeMap(const std::string &path)
    : m_CubeMapTex(LoadCubeMapFromCross(path)) {
    const std::array<float, 108> vertices = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, 
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, 
         1.0f,  1.0f, -1.0f, 
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,  
        -1.0f, -1.0f, -1.0f, 
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, 
        -1.0f,  1.0f,  1.0f,  
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f, 
         1.0f, -1.0f,  1.0f,  
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  
         1.0f,  1.0f, -1.0f, 
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,  
        -1.0f,  1.0f, 1.0f,  
         1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,  
         1.0f, -1.0f, 1.0f,  
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f, 
         1.0f, 1.0f, -1.0f, 
         1.0f, 1.0f,  1.0f,
         1.0f, 1.0f,  1.0f,  
        -1.0f, 1.0f,  1.0f,  
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f, 
        -1.0f, -1.0f,  1.0f,  
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, 
        -1.0f, -1.0f,  1.0f,  
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(nullptr));
}

uint32_t CubeMap::LoadCubeMapFromImages(const std::vector<std::string> &faces) {
    uint32_t texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    int width = 0;
    int height = 0;
    int nrComponents = 0;

    for (uint32_t i = 0; i < faces.size(); i++) {
        unsigned char *data =
            stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (static_cast<bool>(data)) {
            GLenum format = (nrComponents == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                         static_cast<int>(format), width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            Logging::Log(Logging::MessageStates::ERROR,
                         "Cubemap texture failed to load at path: " + faces[i]);
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texID;
}

std::vector<unsigned char>
CubeMap::ExtractSubImage(const unsigned char *const fullImage,
                         const int fullWidth, const int fullHeight,
                         const int xOff, const int yOff, const int faceWidth,
                         const int faceHeight, const int channels) {
    std::vector<unsigned char> faceData(
        static_cast<size_t>(faceWidth * faceHeight * channels));

    for (int y = 0; y < faceHeight; y++) {
        for (int x = 0; x < faceWidth; x++) {
            for (int c = 0; c < channels; c++) {
                int srcIndex =
                    ((((y + yOff) * fullWidth) + (x + xOff)) * channels) + c;
                int dstIndex = (((y * faceWidth) + x) * channels) + c;
                faceData[dstIndex] = fullImage[srcIndex];
            }
        }
    }

    return faceData;
}

uint32_t CubeMap::LoadCubeMapFromCross(const std::string &path) {
    int width = 0;
    int height = 0;
    int nrChannels = 0;
    stbi_set_flip_vertically_on_load(0);
    unsigned char *fullImage =
        stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!static_cast<bool>(fullImage)) {
        Logging::Log(Logging::MessageStates::WARNING, "Failed to load image: " + path);
        return 0;
    }

    uint32_t texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    int faceWidth = width / 4;
    int faceHeight = height / 3;

    struct FaceCoord {
        int x, y;
        FaceCoord(const int x, const int y) : x(x), y(y) {}
    };
    const std::array<FaceCoord, 6> coords = {{
        {2 * faceWidth, 1 * faceHeight}, // +X
        {0 * faceWidth, 1 * faceHeight}, // -X
        {1 * faceWidth, 0 * faceHeight}, // +Y
        {1 * faceWidth, 2 * faceHeight}, // -Y
        {1 * faceWidth, 1 * faceHeight}, // +Z
        {3 * faceWidth, 1 * faceHeight}  // -Z
    }};

    for (int i = 0; i < 6; i++) {
        std::vector<unsigned char> faceData =
            ExtractSubImage(fullImage, width, height, coords.at(i).x,
                            coords.at(i).y, faceWidth, faceHeight, nrChannels);
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, faceWidth,
                     faceHeight, 0, format, GL_UNSIGNED_BYTE, faceData.data());
    }

    stbi_image_free(fullImage);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texID;
}

void CubeMap::Draw(const Shader &shader) const {
    glDepthFunc(GL_LEQUAL);
    shader.Use();
    auto view = GetCam3D().GetViewMatrix();
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    shader.SetMatrix4fv(
        "projection",
        GetCam3D().GetProjectionMatrix(GetScreenWidth() / GetScreenHeight()) *
            skyboxView);
    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapTex);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
} // namespace CPL
