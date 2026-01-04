#pragma once
#include <chrono>
#include <iostream>

class ScopedTimer {
  public:
    explicit ScopedTimer(std::string text) : m_Text(std::move(text)) {
        m_Start = std::chrono::steady_clock::now();
    }

    ~ScopedTimer() {
        m_End = std::chrono::steady_clock::now();
        m_Duration = m_End - m_Start;
        const float ms = m_Duration.count() * 1000.0f;
        std::cout << "------------------------------------------------------\n";
        std::cout << m_Text.c_str() << ": \n";
        std::cout << (ms < 1000 ? ms : ms * 1000.0f)
                  << (ms < 1000 ? " ms\n" : " s\n");
        std::cout << "------------------------------------------------------\n";
    }

  private:
    std::chrono::time_point<std::chrono::steady_clock> m_Start, m_End;
    std::chrono::duration<float> m_Duration{};
    std::string m_Text;
};
