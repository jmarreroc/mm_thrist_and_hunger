#pragma once
#include <windows.h>
#include <cstdint>
#include <atomic>

extern std::atomic<void*> g_var_scraps;
extern std::atomic<void*> g_var_canteen;

bool InstallScrapMidHook();
float GetScraps();
void SetScraps(float scraps);
float GetCanteen();
void SetCanteen(float canteen);