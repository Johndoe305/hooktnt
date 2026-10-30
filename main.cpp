#include <jni.h>
#include <android/log.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include "And64InlineHook.hpp"

#define LOG_TAG "TNT_HOOK"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define OFFSET_GET_RADIUS_MOVABLE 0x28B2118
#define OFFSET_GET_RADIUS_STATIC  0x2A9C8A0
#define OFFSET_SELECT_TARGETS     0x28B0F78
#define OFFSET_EXPLODE            0x28B14A4

static int  (*orig_GetRadius_Movable)(void* self) = nullptr;
static int  (*orig_GetRadius_Static)(void* self) = nullptr;
static void (*orig_SelectTargets)(void* self, int radius) = nullptr;
static void (*orig_Explode)(void* self) = nullptr;

static int hook_GetRadius_Movable(void* self) {
    LOGI("GetRadius Movable chamado!");
    return 1000;
}

static int hook_GetRadius_Static(void* self) {
    LOGI("GetRadius Static chamado!");
    return 1000;
}

static void hook_SelectTargets(void* self, int radius) {
    LOGI("SelectTargets chamado! radius=%d", radius);
    orig_SelectTargets(self, 1000);
}

static void hook_Explode(void* self) {
    LOGI("Explode chamado!");
    orig_Explode(self);
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

    A64HookFunction((void*)(base + OFFSET_GET_RADIUS_MOVABLE), (void*)hook_GetRadius_Movable, (void**)&orig_GetRadius_Movable);
    A64HookFunction((void*)(base + OFFSET_GET_RADIUS_STATIC),  (void*)hook_GetRadius_Static,  (void**)&orig_GetRadius_Static);
    A64HookFunction((void*)(base + OFFSET_SELECT_TARGETS),     (void*)hook_SelectTargets,     (void**)&orig_SelectTargets);
    A64HookFunction((void*)(base + OFFSET_EXPLODE),            (void*)hook_Explode,           (void**)&orig_Explode);

    LOGI("hooks instalados");
}