#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

uniform mat4 projection;
uniform vec3 offset;
uniform mat4 transform;

void main() {
    gl_Position = projection * transform * vec4(aPos + offset, 1.0);
}
