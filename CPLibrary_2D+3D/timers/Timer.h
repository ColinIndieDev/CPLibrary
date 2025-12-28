#pragma once

#include <functional>
#include <utility>

namespace CPL {
    struct Timer {
        float duration;
        float elapsed = 0.0;
        bool loop = false;
        bool finished = false;
        bool paused = false;
        std::function<void(Timer*)> callback;

        Timer(const float time, const bool repeat, std::function<void(Timer*)> cb)
            : duration(time), loop(repeat), callback(std::move(cb)) {}

        void Update(const float delta) {
            if (finished || paused) return;
            elapsed += delta;
            if (elapsed >= duration) {
                callback(this);
                if (loop) {
                    elapsed = 0.0f;
                } else {
                    finished = true;
                }
            }
        }

        void Pause() { paused = true; }
        void Resume() { paused = false; }
        void Stop() { finished = true; }
    };
}
