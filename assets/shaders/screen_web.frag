#version 300 es
precision mediump float;
out vec4 fragColor;

in vec2 TexCoord;

uniform int postProcessingMode;
uniform sampler2D screenTexture;

void Default() {
    vec3 col = texture(screenTexture, TexCoord).rgb;
    fragColor = vec4(col, 1.0);
}
void Inverse() {
    fragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoord)), 1.0);
}
void Grayscale() {
    fragColor = texture(screenTexture, TexCoord);
    float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
    fragColor = vec4(average, average, average, 1.0);
}
void Kernel(int mode) {
    const float offset = 1.0 / 300.0;

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );
    
    float kernel[9];

    if (mode == 4) {
        // === SHARP KERNEL === //
        kernel = float[](
            -1.0, -1.0, -1.0,
            -1.0,  9.0, -1.0,
            -1.0, -1.0, -1.0
        );
    }
    else if (mode == 5) {
        // === EDGE DETECTION KERNEL === //
        kernel = float[](
             1.0,  1.0,  1.0,
             1.0, -8.0,  1.0,
             1.0,  1.0,  1.0
        );
    }
    else {
        // === BLUR KERNEL === //
        kernel = float[](
            1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
            2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
            1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0  
        );
    }

    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++) {
        col += sampleTex[i] * kernel[i];
    }
    
    fragColor = vec4(col, 1.0);
}

void main() {
    if (postProcessingMode == 1) Inverse();
    else if (postProcessingMode == 2) Grayscale();
    else if (postProcessingMode > 2) Kernel(postProcessingMode);
    else Default();
} 
