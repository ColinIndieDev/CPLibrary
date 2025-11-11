#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform vec3 offset;
uniform mat4 transform;

out vec2 FragPos;

void main() {
    FragPos = aPos.xy + offset.xy;
    gl_Position = projection * transform * vec4(aPos + offset, 1.0);
}
