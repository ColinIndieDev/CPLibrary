#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 transform;
uniform vec3 offset;
uniform mat4 projection;

void main() {
    vec3 worldPos = aPos + offset;
    FragPos = vec3(transform * vec4(worldPos, 1.0));

    gl_Position = projection * transform * vec4(worldPos, 1.0);
    TexCoord = aTexCoord;
}
