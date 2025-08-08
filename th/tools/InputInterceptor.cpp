#include "InputInterceptor.h"
#include <cstdint>
#include "mm/hookmgr.h"
#include <cstdio>

typedef SHORT(WINAPI* GetAsyncKeyState_t)(int vKey);
static GetAsyncKeyState_t original_GetAsyncKeyState = nullptr;

// Hook function
SHORT WINAPI Hooked_GetAsyncKeyState(int vKey) {
    if ((vKey == VK_SHIFT || vKey == VK_LSHIFT || vKey == VK_RSHIFT) &&
        InputInterceptor::Instance().IsSprintBlocked()) {
        printf("[InputInterceptor] Shift blockedMinHook init failed.\n");

        return 0;
    }

    return original_GetAsyncKeyState(vKey);
}

InputInterceptor::InputInterceptor() {}

InputInterceptor& InputInterceptor::Instance() {
    static InputInterceptor instance;
    return instance;
}

void InputInterceptor::Init() {
    auto status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
        printf("[InputInterceptor] MinHook init failed: %d (%s)\n", status, MH_StatusToString(status));
        return;
    }

    FARPROC target = GetProcAddress(GetModuleHandleA("user32.dll"), "GetAsyncKeyState");

    if (MH_CreateHook(target, &Hooked_GetAsyncKeyState,
        reinterpret_cast<void**>(&original_GetAsyncKeyState)) != MH_OK) {
        printf("[InputInterceptor] Failed to create hook.\n");
        return;
    }

    if (MH_EnableHook(target) != MH_OK) {
        printf("[InputInterceptor] Failed to enable hook.\n");
        return;
    }

    printf("[InputInterceptor] Hook installed.\n");
}


void InputInterceptor::Shutdown() {
    FARPROC target = GetProcAddress(GetModuleHandleA("user32.dll"), "GetAsyncKeyState");
    MH_DisableHook(target);
    MH_Uninitialize();
}

void InputInterceptor::SetBlockSprint(bool block) {
    blockSprint = block;
}

bool InputInterceptor::IsSprintBlocked() const {
    return blockSprint;
}