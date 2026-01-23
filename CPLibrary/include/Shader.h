#pragma once

#include <string>
#include <glm/gtc/type_ptr.hpp>

namespace CPL {
    struct Color;

    class Shader {
    public:
        Shader() = default;
        Shader(const char* vertexPath, const char* fragmentPath);

        void Use() const;
        void SetBool(const std::string &name, bool value) const;
        void SetInt(const std::string &name, int value) const;
        void SetFloat(const std::string &name, float value) const;
        void SetColor(const std::string &name, const Color& color) const;
        void SetMatrix4fv(const std::string &name, const glm::mat4& matrix) const;
	    void SetVector2f(const std::string &name, const glm::vec2& vec2) const;
        void SetVector3f(const std::string &name, const glm::vec3& vec3) const;
    private:
        uint32_t m_ID;
        static bool m_CheckCompileErrors(uint32_t shader, const std::string& type);
    };
}
