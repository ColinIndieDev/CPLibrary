#pragma once

#include "Logging.h"
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <iostream>

namespace OpenGLDebug {
inline GLenum CheckError(const char *file, int line) {
    GLenum errorCode{};
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            error = "UNKNOWN";
            break;
        }
        std::cerr << "[ERROR]: " << error << " | " << file << " (" << line
                  << ")\n";
    }
    return errorCode;
}

inline void CheckOpenGLError() { CheckError(__FILE__, __LINE__); }

inline void APIENTRY OpenGLDebugOutput(GLenum source, GLenum type, uint32_t id,
                                       GLenum severity, GLsizei length,
                                       const char *message,
                                       const void *userParam) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::cerr << "[ERROR](" << id << "): " << message << "\n";

    std::cerr << "->";
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        std::cerr << "Source: API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cerr << "Source: Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cerr << "Source: Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cerr << "Source: Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cerr << "Source: Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cerr << "Source: Other";
        break;
    default:
        std::cerr << "Source: ???";
        break;
    }
    std::cerr << "\n";

    std::cerr << "->";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cerr << "Type: Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cerr << "Type: Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cerr << "Type: Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cerr << "Type: Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cerr << "Type: Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cerr << "Type: Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cerr << "Type: Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cout << "Type: Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cerr << "Type: Other";
        break;
    default:
        std::cerr << "Type: ???";
        break;
    }
    std::cerr << "\n";

    std::cerr << "-> ";
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        std::cerr << "Severity: HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cerr << "Severity: MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cerr << "Severity: LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cerr << "Severity: NOTIFICTAION";
        break;
    default:
        std::cerr << "Severity: ???";
    }
    std::cerr << "\n";
    std::cerr << "\n";
}

inline void EnableOpenGLDebug() {
    int flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if (static_cast<bool>(flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
        GLint major = 0;
        GLint minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        if (major < 4 || (major == 4 && minor < 3)) {
            Logging::Log(Logging::MessageStates::WARNING, "OpenGL version is older than 4.3 - OpenGL debug output disabled!");
            return;
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                              nullptr, GL_TRUE);
    }
}
} // namespace OpenGLDebug
