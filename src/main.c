#include <stdio.h>
#include <mempool.h>
#include <malloc.h>
#include <stdio.h>
#include <time.h>

#if defined(_WIN64) || defined(_WIN32)

#include <Windows.h>

static UINT64 get_tick_count() {
    LARGE_INTEGER x = {0};
    QueryPerformanceCounter(&x);
    return x.QuadPart;
}

static double get_ms_diff(UINT64 v0, UINT64 v1) {
    double diff = v1 - v0;
    LARGE_INTEGER x = {0};
    QueryPerformanceFrequency(&x);
    double freq = x.QuadPart;
    double res = (diff / freq) * 1000.0;
    return res;
}

#define TYPESIZE 0x10
#define AMOUNT 0x100000

static volatile void* a;

#define BENCHMARK_START(N) \
    printf("starting benchmark \"%s\"\n", N); \
    UINT64 start = get_tick_count();

#define BENCHMARK_END \
    double diff = get_ms_diff(start, get_tick_count()); \
    printf("finished benchmark in %f ms\n", diff);

static void mempool_test_reserve() {
    BENCHMARK_START("mempool_test_reserve");
    mempool m = mempool_create(TYPESIZE, AMOUNT);
    for (size_t i = 0; i < AMOUNT; ++i) {
        a = mempool_new(m);
    }
    BENCHMARK_END;
    mempool_release(m);
}

static void mempool_test_noreserve() {
    BENCHMARK_START("mempool_test_noreserve");
    mempool m = mempool_create(TYPESIZE, 0x20);
    for (size_t i = 0; i < AMOUNT; ++i) {
        a = mempool_new(m);
    }
    BENCHMARK_END;
    mempool_release(m);
}

static void default_test() {
    BENCHMARK_START("default_test");
    for (size_t i = 0; i < AMOUNT; ++i) {
        a = malloc(TYPESIZE);
    }
    BENCHMARK_END;
}

static void mempool_test_free() {
    BENCHMARK_START("mempool_test_free");
    mempool m = mempool_create(TYPESIZE, 0x20);
    for (size_t i = 0; i < AMOUNT; ++i) {
        a = mempool_new(m);
        mempool_delete(m, a);
    }
    BENCHMARK_END;
    mempool_release(m);
}

static void default_test_free() {
    BENCHMARK_START("default_test_free");
    for (size_t i = 0; i < AMOUNT; ++i) {
        a = malloc(TYPESIZE);
        free(a);
    }
    BENCHMARK_END;
}

int main() {
    default_test();
    mempool_test_reserve();
    mempool_test_noreserve();
    mempool_test_free();
    default_test_free();
    return 0;
}

#else

int main() {
    printf("No linux tests, no big deal...\n");
    printf("I should've written the benchmark using c++ and std::chrono, but nevermind\n");
    printf("I only had QueryPerformanceCounter :(\n");
    return 0;
}

#endif