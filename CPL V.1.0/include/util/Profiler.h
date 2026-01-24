#pragma once

#include <atomic>
#include <pthread.h>

class Profiler {
  public:
    static std::atomic<size_t> heapUsed;

    static size_t GetHeapUsed() { return heapUsed; }

    static void RecordAlloc(void *ptr, size_t size);
    static void RecordDealloc(void *ptr);

    static size_t GetStackSize() {
        pthread_attr_t attr;
        pthread_getattr_np(pthread_self(), &attr);
        size_t stackSize = 0;
        pthread_attr_getstacksize(&attr, &stackSize);
        pthread_attr_destroy(&attr);
        return stackSize;
    }

    static size_t GetStackUsed() {
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
};
