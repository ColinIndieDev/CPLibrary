#version 330 core

in vec2 TexCoord;

uniform sampler2D tex;

void main() {
    float a = texture(tex, TexCoord).a;
    if (a < 0.1) discard;
}
