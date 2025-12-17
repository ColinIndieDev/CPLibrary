#include "TimerManager.h"

namespace CPL {
std::vector<Timer> TimerManager::timers{};

void TimerManager::Update(const float delta) {
    for (auto &t : timers)
        t.Update(delta);
    std::erase_if(timers, [](auto &t) { return t.finished; });
}

Timer *TimerManager::AddTimer(float duration, bool loop,
                              const std::function<void(Timer*)> &cb) {
    timers.push_back({duration, loop, cb});
    return &timers.back();
}

void TimerManager::StopTimers() {
    for (auto &t : timers) {
        t.Pause();
    }
}

void TimerManager::ClearTimers() { timers.clear(); }
} // namespace CPL
