#pragma once

#include "../CPL.h"
#include "Timer.h"
#include <functional>

namespace CPL {
struct Timer;

class TimerManager {
  public:
    static void Update(float delta);
    static Timer *AddTimer(float duration, bool loop,
                           const std::function<void(Timer*)> &cb);
    static void StopTimers();
    static void ClearTimers();

  private:
    static std::vector<Timer> s_Timers;
};
} // namespace CPL
