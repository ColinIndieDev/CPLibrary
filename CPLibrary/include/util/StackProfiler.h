#pragma once

#include <pthread.h>

namespace StackProfiler {
size_t GetStackSize() {
    pthread_attr_t attr;
    pthread_getattr_np(pthread_self(), &attr);
    size_t stackSize = 0;
    pthread_attr_getstacksize(&attr, &stackSize);
    pthread_attr_destroy(&attr);
    return stackSize;
}

size_t GetStackUsed() {
    pthread_attr_t attr;
    pthread_getattr_np(pthread_self(), &attr);
    void *stackBase = nullptr;
    size_t stackSize = 0;
    pthread_attr_getstack(&attr, &stackBase, &stackSize);
    pthread_attr_destroy(&attr);
    char marker{};
    void *current = &marker;
    return static_cast<size_t>(static_cast<char *>(stackBase) + stackSize -
                               static_cast<char *>(current));
}
} // namespace StackProfiler
