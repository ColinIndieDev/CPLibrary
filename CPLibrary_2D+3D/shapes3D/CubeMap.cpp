#include "CubeMap.h"
#include "../Shader.h"
#include <stb_image.h>

namespace CPL {
CubeMap::CubeMap(const std::string &path) {
    const float vertices[] = {
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
        -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);

    cubeMapTexture = LoadCubeMapFromCross(path);
}

unsigned int
CubeMap::LoadCubeMapFromImages(const std::vector<std::string> &faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data =
            stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format = (nrComponents == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            Logging::Log(2,
                         "Cubemap texture failed to load at path: " + faces[i]);
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned char *CubeMap::ExtractSubImage(unsigned char *fullImage, int fullWidth,
                                        int fullHeight, int xOffset,
                                        int yOffset, int faceWidth,
                                        int faceHeight, int channels) {
    unsigned char *faceData =
        new unsigned char[faceWidth * faceHeight * channels];

    for (int y = 0; y < faceHeight; y++) {
        for (int x = 0; x < faceWidth; x++) {
            for (int c = 0; c < channels; c++) {
                int srcIndex =
                    ((y + yOffset) * fullWidth + (x + xOffset)) * channels + c;
                int dstIndex = (y * faceWidth + x) * channels + c;
                faceData[dstIndex] = fullImage[srcIndex];
            }
        }
    }

    return faceData;
}

unsigned int CubeMap::LoadCubeMapFromCross(const std::string &path) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char *fullImage =
        stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!fullImage) {
        std::cout << "Failed to load image: " << path << std::endl;
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int faceWidth = width / 4;
    int faceHeight = height / 3;

    struct FaceCoord {
        int x, y;
    };
    FaceCoord coords[6] = {
        {2 * faceWidth, 1 * faceHeight}, // +X
        {0 * faceWidth, 1 * faceHeight}, // -X
        {1 * faceWidth, 0 * faceHeight}, // +Y
        {1 * faceWidth, 2 * faceHeight}, // -Y
        {1 * faceWidth, 1 * faceHeight}, // +Z
        {3 * faceWidth, 1 * faceHeight}  // -Z
    };

    for (int i = 0; i < 6; i++) {
        unsigned char *faceData =
            ExtractSubImage(fullImage, width, height, coords[i].x, coords[i].y,
                            faceWidth, faceHeight, nrChannels);
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, faceWidth,
                     faceHeight, 0, format, GL_UNSIGNED_BYTE, faceData);
        delete[] faceData;
    }

    stbi_image_free(fullImage);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void CubeMap::Draw(const Shader &shader) {
    glDepthFunc(GL_LEQUAL);
    shader.Use();
    auto view = GetCamera3D().GetViewMatrix();
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    shader.SetMatrix4fv(
        "projection",
        GetCamera3D().GetProjectionMatrix(GetScreenWidth() / GetScreenHeight()) *
            skyboxView);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
} // namespace CPL
