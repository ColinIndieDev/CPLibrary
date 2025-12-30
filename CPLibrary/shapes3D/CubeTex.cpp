#include "CubeTex.h"
#include "../CPL.h"
#include "../Shader.h"
#include "../shapes2D/Texture2D.h"
#include "Cube.h"

namespace CPL {
CubeTex::CubeTex(const glm::vec3 pos, const glm::vec3 size, const Color color)
    : position(pos), size(size), color(color) {
    float sx = size.x / 2.0f;
    float sy = size.y / 2.0f;
    float sz = size.z / 2.0f;

    const float vertices[] = {
        // Back face (Z-)
        -sx, -sy, -sz, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, sx, sy, -sz, 0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f, sx, -sy, -sz, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, sx, sy,
        -sz, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, -sx, -sy, -sz, 0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -sx, sy, -sz, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

        // Front face (Z+)
        -sx, -sy, sz, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, sx, -sy, sz, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, sx, sy, sz, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, sx, sy, sz,
        0.0f, 0.0f, 1.0f, 1.0f, 1.0f, -sx, sy, sz, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -sx, -sy, sz, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        // Left face (X–)
        -sx, sy, sz, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -sx, sy, -sz, -1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -sx, -sy, -sz, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -sx,
        -sy, -sz, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -sx, -sy, sz, -1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -sx, sy, sz, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        // Right face (X+)
        sx, sy, sz, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, sx, -sy, -sz, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, sx, sy, -sz, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, sx, -sy,
        -sz, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, sx, sy, sz, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, sx, -sy, sz, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        // Bottom face (Y–)
        -sx, -sy, -sz, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, sx, -sy, -sz, 0.0f, -1.0f,
        0.0f, 1.0f, 1.0f, sx, -sy, sz, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, sx, -sy,
        sz, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -sx, -sy, sz, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, -sx, -sy, -sz, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        // Top face (Y+)
        -sx, sy, -sz, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, sx, sy, sz, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, sx, sy, -sz, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, sx, sy, sz,
        0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -sx, sy, -sz, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -sx, sy, sz, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    InitAtlas();
}
void CubeTex::InitAtlas() {
    float sx = size.x / 2.0f;
    float sy = size.y / 2.0f;
    float sz = size.z / 2.0f;

    // ✅ Texture Atlas Layout (3x2 Grid):
    // Row 0: [Right] [Top]   [Left]
    // Row 1: [Front] [Bottom][Back]
    float w = 1.0f / 3.0f; 
    float h = 1.0f / 2.0f;

    const float atlasVertices[] = {
        // Back face (Z-) - Position (2,1)
        -sx, -sy, -sz, 0.0f, 0.0f, -1.0f, 2*w, 1.0f,
         sx,  sy, -sz, 0.0f, 0.0f, -1.0f, 3*w, 0.5f,
         sx, -sy, -sz, 0.0f, 0.0f, -1.0f, 3*w, 1.0f,
         sx,  sy, -sz, 0.0f, 0.0f, -1.0f, 3*w, 0.5f,
        -sx, -sy, -sz, 0.0f, 0.0f, -1.0f, 2*w, 1.0f,
        -sx,  sy, -sz, 0.0f, 0.0f, -1.0f, 2*w, 0.5f,

        // Front face (Z+) - Position (1,0)
        -sx, -sy, sz, 0.0f, 0.0f, 1.0f, 1*w, 0.5f,
         sx, -sy, sz, 0.0f, 0.0f, 1.0f, 2*w, 0.5f,
         sx,  sy, sz, 0.0f, 0.0f, 1.0f, 2*w, 0.0f,
         sx,  sy, sz, 0.0f, 0.0f, 1.0f, 2*w, 0.0f,
        -sx,  sy, sz, 0.0f, 0.0f, 1.0f, 1*w, 0.0f,
        -sx, -sy, sz, 0.0f, 0.0f, 1.0f, 1*w, 0.5f,

        // Left face (X-) - Position (2,0)
        -sx, sy,  sz, -1.0f, 0.0f, 0.0f, 3*w, 0.0f,
        -sx, sy, -sz, -1.0f, 0.0f, 0.0f, 2*w, 0.0f,
        -sx, -sy, -sz, -1.0f, 0.0f, 0.0f, 2*w, 0.5f,
        -sx, -sy, -sz, -1.0f, 0.0f, 0.0f, 2*w, 0.5f,
        -sx, -sy, sz, -1.0f, 0.0f, 0.0f, 3*w, 0.5f,
        -sx, sy,  sz, -1.0f, 0.0f, 0.0f, 3*w, 0.0f,

        // Right face (X+) - Position (0,0)
        sx, sy,  sz, 1.0f, 0.0f, 0.0f, 1*w, 0.0f,
        sx, -sy, -sz, 1.0f, 0.0f, 0.0f, 0*w, 0.5f,
        sx, sy, -sz, 1.0f, 0.0f, 0.0f, 0*w, 0.0f,
        sx, -sy, -sz, 1.0f, 0.0f, 0.0f, 0*w, 0.5f,
        sx, sy,  sz, 1.0f, 0.0f, 0.0f, 1*w, 0.0f,
        sx, -sy, sz, 1.0f, 0.0f, 0.0f, 1*w, 0.5f,

        // Bottom face (Y-) - Position (0,1)
        -sx, -sy, -sz, 0.0f, -1.0f, 0.0f, 0*w, 0.5f,
         sx, -sy, -sz, 0.0f, -1.0f, 0.0f, 1*w, 0.5f,
         sx, -sy,  sz, 0.0f, -1.0f, 0.0f, 1*w, 1.0f,
         sx, -sy,  sz, 0.0f, -1.0f, 0.0f, 1*w, 1.0f,
        -sx, -sy,  sz, 0.0f, -1.0f, 0.0f, 0*w, 1.0f,
        -sx, -sy, -sz, 0.0f, -1.0f, 0.0f, 0*w, 0.5f,

        // Top face (Y+) - Position (1,1)
        -sx, sy, -sz, 0.0f, 1.0f, 0.0f, 1*w, 0.5f,
         sx, sy,  sz, 0.0f, 1.0f, 0.0f, 2*w, 1.0f,
         sx, sy, -sz, 0.0f, 1.0f, 0.0f, 2*w, 0.5f,
         sx, sy,  sz, 0.0f, 1.0f, 0.0f, 2*w, 1.0f,
        -sx, sy, -sz, 0.0f, 1.0f, 0.0f, 1*w, 0.5f,
        -sx, sy,  sz, 0.0f, 1.0f, 0.0f, 1*w, 1.0f
    };

    glGenVertexArrays(1, &VAOAtlas);
    glGenBuffers(1, &VBOAtlas);
    glBindVertexArray(VAOAtlas);
    glBindBuffer(GL_ARRAY_BUFFER, VBOAtlas);
    glBufferData(GL_ARRAY_BUFFER, sizeof(atlasVertices), atlasVertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

CubeTex::~CubeTex() {
    if (VAO != 0 && glIsVertexArray(VAO)) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0 && glIsBuffer(VBO)) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAOAtlas != 0 && glIsVertexArray(VAOAtlas)) {
        glDeleteVertexArrays(1, &VAOAtlas);
        VAOAtlas = 0;
    }
    if (VBOAtlas != 0 && glIsBuffer(VBOAtlas)) {
        glDeleteBuffers(1, &VBOAtlas);
        VBOAtlas = 0;
    }
}

void CubeTex::Draw(const Shader &shader, const Texture2D *texture) const {
    auto transform = glm::mat4(1.0f);

    glm::mat4 view = GetCam3D().GetViewMatrix();
    glm::mat4 projection =
        GetCam3D().GetProjectionMatrix(GetScreenWidth() / GetScreenHeight());
    shader.SetMatrix4fv("projection", projection * view);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(position));
    shader.SetColor("inputColor", color);
    shader.SetInt("ourTexture", 0);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void CubeTex::DrawDepth(const Shader &shader, const Texture2D *texture) const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    shader.Use();
    shader.SetMatrix4fv("model", model);
    shader.SetInt("ourTexture", 0);
    
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void CubeTex::DrawAtlas(const Shader &shader,
                        const Texture2D *atlasTexture) const {
    auto transform = glm::mat4(1.0f);

    glm::mat4 view = GetCam3D().GetViewMatrix();
    glm::mat4 projection =
        GetCam3D().GetProjectionMatrix(GetScreenWidth() / GetScreenHeight());
    shader.SetMatrix4fv("projection", projection * view);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(position));
    shader.SetColor("inputColor", color);
    shader.SetInt("ourTexture", 0);

    glBindVertexArray(VAOAtlas);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTexture->texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void CubeTex::DrawDepthAtlas(const Shader &shader,
                             const Texture2D *atlasTexture) const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    shader.Use();
    shader.SetMatrix4fv("model", model);
    shader.SetInt("ourTexture", 0);

    glBindVertexArray(VAOAtlas);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTexture->texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace CPL
