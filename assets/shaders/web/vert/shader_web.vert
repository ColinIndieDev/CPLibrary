#version 300 es
precision mediump float;
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform vec3 offset;
uniform mat4 transform;

void main() {
    gl_Position = projection * transform * vec4(aPos + offset, 1.0);
}
