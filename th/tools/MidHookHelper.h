#pragma once
#include <cstdint>

// Helpers que ya usas en ScrapInlineHook
extern uint8_t* FindPattern(uint8_t* base, size_t size, const char* pattern, const char* mask);
extern void* AllocNear(void* target, size_t size);
extern bool WriteMem(void* dst, const void* src, size_t size);
