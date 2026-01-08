#include "../include/Shader.h"
#include "../include/CPL.h"
#include "../include/util/Logging.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace CPL {
Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream;
        std::stringstream fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &e) {
        Logging::Log(Logging::MessageStates::ERROR,
                     "File not successfully read: " + std::string(e.what()));
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    uint32_t vertex = 0;
    uint32_t fragment = 0;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    m_CheckCompileErrors(vertex, "VERTEX");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    m_CheckCompileErrors(fragment, "FRAGMENT");
    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    m_CheckCompileErrors(m_ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::Use() const { glUseProgram(m_ID); }

void Shader::SetBool(const std::string &name, const bool value) const {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()),
                static_cast<int>(value));
}

void Shader::SetInt(const std::string &name, const int value) const {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, const float value) const {
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::SetColor(const std::string &name, const Color &color) const {
    glUniform4f(glGetUniformLocation(m_ID, name.c_str()), color.r, color.g,
                color.b, color.a);
}

void Shader::SetMatrix4fv(const std::string &name,
                          const glm::mat4 &matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(matrix));
}

void Shader::SetVector2f(const std::string &name, const glm::vec2 &vec2) const {
    glUniform2f(glGetUniformLocation(m_ID, name.c_str()), vec2.x, vec2.y);
}

void Shader::SetVector3f(const std::string &name, const glm::vec3 &vec3) const {
    glUniform3f(glGetUniformLocation(m_ID, name.c_str()), vec3.x, vec3.y,
                vec3.z);
}

void Shader::m_CheckCompileErrors(const uint32_t shader,
                                  const std::string &type) {
    int success = 0;
    std::array<char, 1024> infoLog{};
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!static_cast<bool>(success)) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog.data());
            Logging::Log(Logging::MessageStates::ERROR,
                         "Shader compilation error: " + type + "\n" + "-> " +
                             std::string(infoLog.data()));
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!static_cast<bool>(success)) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog.data());
            Logging::Log(Logging::MessageStates::ERROR,
                         "Program linking error of type: " + type + "\n" +
                             std::string(infoLog.data()));
        }
    }
}
} // namespace CPL
