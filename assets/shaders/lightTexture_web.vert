#version 300 es
precision mediump float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 projection;
uniform vec3 offset;
uniform mat4 transform;

out vec2 FragPos;
out vec2 TexCoord;

void main() {
    FragPos = aPos.xy + offset.xy;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    gl_Position = projection * transform * vec4(aPos + offset, 1.0);
}
