#version 330 core

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
    float a = texture(ourTexture, TexCoord).a;
    if (a < 0.1) discard;
}
