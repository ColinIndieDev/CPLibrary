#version 330 core

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
    float alpha = texture(ourTexture, TexCoord).a;
    if (alpha < 0.1)
        discard;    
}
