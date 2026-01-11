#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;
in vec4 FragPosLightSpace;

struct PointLight {
    vec3 position;
    vec4 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform float shininess;
uniform int numPointLights;
uniform PointLight pointLights[32];
uniform DirectionalLight dirLight;
uniform vec3 viewPos;
uniform vec4 inputColor;
uniform sampler2D ourTexture;
uniform sampler2D shadowMap;  

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    vec3 ambient = light.ambient;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec;
   
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    float distance = length(light.position - fragPos);
    float attenuation = light.intensity / (light.constant + light.linear * distance + 
                                           light.quadratic * (distance * distance));
    
    vec3 lightColor = light.color.rgb / 255.0;
    
    float ambientStrength = 0.1;
    float specularStrength = 0.5;
    vec3 result = (ambientStrength + diff + spec * specularStrength) * lightColor * attenuation;
    
    return result;
}

void main() {
    vec4 textureColor = texture(ourTexture, TexCoord);
    if (textureColor.a < 0.1) discard;
    
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 lighting = CalcDirLight(dirLight, normal, viewDir);
    
    for(int i = 0; i < numPointLights; i++)
        lighting += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
    
    vec3 lightDir = normalize(-dirLight.direction);
    float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);
    
    vec3 ambient = dirLight.ambient;
    vec3 diffuseSpecular = lighting - ambient;
    lighting = ambient + (1.0 - shadow) * diffuseSpecular;
    
    vec3 baseColor = inputColor.rgb / 255 * textureColor.rgb;
    vec3 result = baseColor * lighting;

    vec3 fogColor = vec3(0.4, 0.8, 1.0);
    float dist = length(FragPos.xz - viewPos.xz);
    float fogFactor = clamp((45.0 - dist) / (45.0 - 39.0), 0.0, 1.0);
    float fogFactorAlpha = clamp((45.0 - dist) / (45.0 - 44.0), 0.0, 1.0);

    vec3 finalColor = mix(fogColor, result, fogFactor);
    float finalAlpha = mix(0, 255, fogFactorAlpha);
    
    FragColor = vec4(finalColor, inputColor.a / 255 * textureColor.a * finalAlpha);
}
