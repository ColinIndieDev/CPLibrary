#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
 
struct PointLight {
    vec3 position;
    vec4 color;
    float intensity;
    
    float constant;
    float linear;
    float quadratic;
	
    /*
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
     */
};
struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform float shininess;
uniform int numPointLights;
uniform PointLight pointLights[32]; // Maximum are 32
uniform DirectionalLight dirLight;
uniform vec3 viewPos; 
uniform vec4 inputColor;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    vec3 ambient = light.ambient * (inputColor.rgb / 255);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * (inputColor.rgb / 255);
    // specular shading
    vec3 reflectDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec * (inputColor.rgb / 255);
   
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * (light.color.rgb / 255);

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * (light.color.rgb / 255);
    // specular shading
    float specularStrength = 0.5;
    vec3 reflectDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * (light.color.rgb / 255);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main() {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, normal, viewDir);

    for(int i = 0; i < numPointLights; i++)
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
        
    vec4 finalResult = vec4(result, 1.0) * (inputColor / 255);
    FragColor = finalResult;
}
