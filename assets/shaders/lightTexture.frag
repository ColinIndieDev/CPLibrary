#version 330 core
out vec4 FragColor;

in vec2 FragPos;
in vec2 TexCoord;

uniform vec4 inputColor;
uniform float ambient;
uniform sampler2D ourTexture;

struct PointLight {
    vec2 position;
    float radius;
    float intensity;
    vec4 color;
};
uniform int numPointLights;
uniform PointLight pointLights[32]; // Maximum are 32 point lights

vec3 CalcPointLight(PointLight l, vec2 fragPos, float ambient) {
    float dist = length(l.position - fragPos);
    float falloff = clamp(1.0 - dist / l.radius, 0.0, 1.0);

    vec3 lightCol = l.color.rgb / 255.0 * l.intensity;

    falloff = falloff * falloff; // optional smooth

    return (ambient + falloff) * lightCol;
}

void main() 
{
    vec4 textureColor = texture(ourTexture, TexCoord);
    if (textureColor.a < 0.1) discard;

    vec3 result = vec3(ambient);

    for (int i = 0; i < numPointLights; i++) {
        result += CalcPointLight(pointLights[i], FragPos, ambient); 
    }

    vec3 obj = inputColor.rgb / 255.0 * textureColor.rgb;
    vec3 finalColor = obj * result;

    FragColor = vec4(finalColor, inputColor.a / 255.0 * textureColor.a);
}
