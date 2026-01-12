#include "../../include/util/Profiler.h"

std::atomic<size_t> Profiler::heapUsed{0};

void *operator new(size_t sz) {
    Profiler::heapUsed += sz;
    return std::malloc(sz);
}

void operator delete(void *ptr, size_t sz) noexcept {
    Profiler::heapUsed -= sz;
    std::free(ptr);
}
