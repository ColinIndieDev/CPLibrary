#include "../../include/timer/TimerManager.h"

namespace CPL {
std::vector<Timer> TimerManager::s_Timers{};

void TimerManager::Update(const float delta) {
    for (auto &t : s_Timers)
        t.Update(delta);
    s_Timers.erase(std::remove_if(s_Timers.begin(), s_Timers.end(),
                                  [](auto &t) { return t.finished; }),
                   s_Timers.end());
}

Timer *TimerManager::AddTimer(float duration, bool loop,
                              const std::function<void(Timer *)> &cb) {
    s_Timers.emplace_back(duration, loop, cb);
    return &s_Timers.back();
}

void TimerManager::StopTimers() {
    for (auto &t : s_Timers) {
        t.Pause();
    }
}

void TimerManager::ClearTimers() { s_Timers.clear(); }
} // namespace CPL
