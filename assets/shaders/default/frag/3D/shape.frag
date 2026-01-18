#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;

uniform vec4 objColor;
uniform sampler2D tex;

uniform bool useFog;
uniform float fogStart;
uniform float fogEnd;
uniform vec4 fogColor;

uniform vec3 viewPos;

void main() {
    vec4 texColor = texture(tex, TexCoord);
    if (texColor.a < 0.1) discard;

    vec3 result = objColor.rgb / 255 * texColor.rgb;

    float dist = length(FragPos.xz - viewPos.xz);

    float fogFactor = clamp((fogStart - dist) / (fogStart - fogEnd), 0.0, 1.0);

    vec3 finalColor = useFog ? mix(fogColor.rgb / 255, result, fogFactor) : result;

    FragColor = vec4(finalColor, objColor.a / 255 * texColor.a);
}
