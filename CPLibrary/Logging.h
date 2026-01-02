#pragma once
#include <iostream>
#include <string>
#include <cstdint>

namespace Logging {
enum class MessageStates : uint8_t {
    INFO,
    WARNING,
    ERROR,
};
inline void Log(const MessageStates &warnLevel, const std::string &text) {
    if (warnLevel == MessageStates::INFO)
        std::cout << "[INFO]: " << text << "\n";
    else if (warnLevel == MessageStates::WARNING)
        std::cout << "[WARNING]: " << text << "\n";
    else
        std::cerr << "[ERROR]: " << text << "\n";
}
} // namespace Logging
