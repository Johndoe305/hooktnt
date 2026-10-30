#include <jni.h>
#include <android/log.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include "And64InlineHook.hpp"

#define LOG_TAG "TNT_HOOK"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define OFFSET_GET_RADIUS_AD     0x2A9B71C
#define OFFSET_GET_RADIUS_STATIC 0x2A9C8A0

static int (*orig_Ad)(void* self) = nullptr;
static int (*orig_Static)(void* self) = nullptr;

static int hook_Ad(void* self) {
    LOGI("Ad hook!");
    return 1000;
}

static int hook_Static(void* self) {
    LOGI("Static hook!");
    return 1000;
}

static uintptr_t get_lib_base(const char* name) {
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) return 0;
    char line[512];
    uintptr_t addr = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, name)) {
            sscanf(line, "%lx", &addr);
            break;
        }
    }
    fclose(f);
    return addr;
}

__attribute__((constructor))
static void on_load() {
    LOGI("lib carregada");

    uintptr_t base = 0;
    for (int i = 0; i < 50; i++) {
        base = get_lib_base("libil2cpp.so");
        if (base) break;
        usleep(100000);
    }
    if (!base) { LOGI("base nao achada"); return; }

    LOGI("base: %lx", base);

    A64HookFunction((void*)(base + OFFSET_GET_RADIUS_AD),     (void*)hook_Ad,     (void**)&orig_Ad);
    A64HookFunction((void*)(base + OFFSET_GET_RADIUS_STATIC), (void*)hook_Static, (void**)&orig_Static);

    LOGI("hooks instalados");
}
