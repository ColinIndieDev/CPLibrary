#include "../../include/util/Profiler.h"
#include <cstdlib>
#include <unordered_map>
#include <new>
#include <memory>

std::atomic<size_t> Profiler::heapUsed{0};

static thread_local bool g_InProfiling = false;

static std::unordered_map<void *, size_t> *GetAllocationsPtr() {
    static thread_local std::unique_ptr<std::unordered_map<void *, size_t>> allocations =
        nullptr;

    if (!static_cast<bool>(allocations)) {
        g_InProfiling = true;
        allocations = std::make_unique<std::unordered_map<void *, size_t>>();
        g_InProfiling = false;
    }

    return allocations.get();
}

void Profiler::RecordAlloc(void *ptr, size_t size) {
    if (g_InProfiling)
        return;

    auto *allocations = GetAllocationsPtr();
    if (!static_cast<bool>(allocations))
        return;

    g_InProfiling = true;
    (*allocations)[ptr] = size;
    heapUsed += size;
    g_InProfiling = false;
}

void Profiler::RecordDealloc(void *ptr) {
    if (g_InProfiling)
        return;

    auto *allocations = GetAllocationsPtr();
    if (!static_cast<bool>(allocations))
        return;

    g_InProfiling = true;
    auto it = allocations->find(ptr);
    if (it != allocations->end()) {
        heapUsed -= it->second;
        allocations->erase(it);
    }
    g_InProfiling = false;
}

void *operator new(size_t sz) {
    void *ptr = std::malloc(sz);
    if (!static_cast<bool>(ptr))
        throw std::bad_alloc();

    Profiler::RecordAlloc(ptr, sz);
    return ptr;
}

void operator delete(void *ptr) noexcept {
    if (!static_cast<bool>(ptr))
        return;
    Profiler::RecordDealloc(ptr);
    std::free(ptr);
}

void operator delete(void *ptr, size_t sz) noexcept {
    if (!static_cast<bool>(ptr))
        return;

    if (!g_InProfiling) {
        Profiler::heapUsed -= sz;
        auto *allocations = GetAllocationsPtr();
        if (static_cast<bool>(allocations)) {
            g_InProfiling = true;
            allocations->erase(ptr);
            g_InProfiling = false;
        }
    }
    std::free(ptr);
}

void *operator new[](size_t sz) { return operator new(sz); }

void operator delete[](void *ptr) noexcept { operator delete(ptr); }

void operator delete[](void *ptr, size_t sz) noexcept {
    operator delete(ptr, sz);
}
