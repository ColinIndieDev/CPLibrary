#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 projection;
uniform vec3 offset;
uniform mat4 transform;

void main() {
    FragPos = vec3(transform * vec4(aPos + offset, 1.0));
    Normal = mat3(transpose(inverse(transform))) * aNormal;  
    
    gl_Position = projection * vec4(FragPos, 1.0);
}
