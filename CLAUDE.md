# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Thirst & Hunger is a Mad Max (2015) game mod that adds survival mechanics via inline memory hooks. It produces a `.asi` plugin DLL loaded by the game through Ultimate ASI Loader. The mod implements health decay, sanity management, fuel consumption, and roguelite penalties.

## Build Commands

This is a Visual Studio 2022 project (v143 toolset) targeting x64 Windows. Build from the Developer Command Prompt or PowerShell:

```bash
# Build Release x64
msbuild mm_plugin.sln /p:Configuration=Release /p:Platform=x64

# Build Debug x64
msbuild mm_plugin.sln /p:Configuration=Debug /p:Platform=x64
```

The output `.asi` file is placed in the game's `scripts/` directory (configured in `.vcxproj` `<OutDir>`). The game must be installed at the expected path for automatic deployment.

**Prerequisites:** Visual Studio 2022 with C++桌面开发 workload, SDL2 and SDL2_mixer libraries in `lib/`, Boost 1.86.0 via NuGet.

## Architecture

### Two-Layer Design

**`mm/` — Core Hooking Infrastructure**
- `hookmgr.cpp/h` — MinHook-based inline hooking with `ADDRESS(gog, steam)` macro for dual-build support (GOG vs Steam addresses)
- `imgui/` — IMGUI integration with DX11 detour for in-game UI rendering
- `minhook/` — Bundled MinHook library
- `game/` — Game object interfaces (`CCharacter`, `CVehicle`) using `CMETHOD`/`SMETHOD` macros to call game functions at hardcoded addresses

**`th/` — Mod Logic (Thirst & Hunger)**
- `decay/` — Passive health decay over time
- `sanity/` — Sanity system with flashbacks, HUD, and audio director
- `fuel/` — Fuel consumption hooks and management
- `scrap/` — Scrap collection tracking
- `canteen/` — Canteen state management
- `vehicle/` — Vehicle damage → player/fuel penalty proxy
- `roguelite/` — Death and loading screen penalties
- `tools/` — Scanner, input interception, INI reader, SDL audio player

### Key Patterns

**Dual Address Macro:** `ADDRESS(gog_addr, steam_addr)` selects the correct function address based on game version detection via `HookMgr::isSteam`/`isGOG` flags set at startup.

**Hook Installation:** Two patterns exist:
1. Standard MinHook: `HookMgr::Install(address, hook_func, original_func_ptr)` — for function hooks
2. Inline/Mid hooks: Custom `InstallScrapMidHook()` / `InstallFuelMidHook()` — patch specific instructions (e.g., `movss [rbx+0x0C]`) in `th/*/FuelInlineHook.cpp`, `th/*/ScrapInlineHook.cpp`

**Config Singleton:** `Config::instance()` loads `etc/th/config.ini` at plugin attach. Access via `cfg.decay()`, `cfg.sanity()`, `cfg.roguelite()`.

**Plugin Entry:** `plugin.cpp` defines `DLLATTATCH` (DllMain → `HookMgr::Initialize()` → `PluginAttach()`). Hooks install after a 5-second delay in a detached thread to allow game initialization.

### Runtime Flow

1. DLL loads → `HookMgr::Initialize()` (MinHook init)
2. `PluginAttach()` loads config, spawns delayed hook thread
3. After 5s: `PluginHooks()` installs main hook at game's update loop (`0x1404C6670`/`0x1420DEAC0`)
4. Each frame: `Thirst__Hunger()` calls `DecayManager`, `SanityManager`, `VehicleDamageProxy`, `FuelManager`, etc.
5. Inline hooks intercept specific game operations (fuel consumption, scrap pickup)

## Configuration

Runtime settings in `etc/th/config.ini` — no rebuild needed. Sections: `[decay]`, `[sanity]`, `[roguelite]`. Assets (textures, sounds) in `etc/th/textures/` and `etc/th/sounds/`.
