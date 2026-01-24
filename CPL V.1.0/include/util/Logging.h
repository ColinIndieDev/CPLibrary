#pragma once
#include <cstdint>
#include <iostream>
#include <string>

namespace Logging {
enum class MessageStates : uint8_t {
    INFO,
    WARNING,
    ERROR,
};
inline void Log(const MessageStates &warnLevel, const std::string &text) {
    if (warnLevel == MessageStates::INFO) {
        std::cout << "[INFO]: " << text << "\n";
        std::cout << "\n";
    } else if (warnLevel == MessageStates::WARNING) {
        std::cout << "[WARNING]: " << text << "\n";
        std::cout << "\n";
    } else {
        std::cerr << "[ERROR]: " << text << "\n";
        std::cout << "\n";
    }
}
} // namespace Logging
