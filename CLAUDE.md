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


# Project Analysis: Thirst & Hunger (MadMax Mod)

## Overview
This is a Grand Theft Auto: San Andreas mod ("MadMax") that implements survival mechanics through direct memory manipulation via inline hooks. The mod modifies game behavior at the assembly level to create challenging survival gameplay.

## Architecture & Memory Hooks

### Core Hook System (`mm/hookmgr.cpp`)
- Uses **MinHook** library for inline function hooking
- Patches GTA:SA's main game loop at address `0x1404C6670` / `0x1420DEAC0`
- Main hook function: `Thirst__Hunger` (called every game frame with delta time)
- Runs in a separate thread with 5-second delay before hook installation

### Survival Mechanics Managers

#### 1. Decay Manager (`th/decay/DecayManager.cpp`)
- **Memory Access**: Directly accesses `CCharacter` health at offset `0x184`
- **Hook Target**: CharacterManager's `GetPlayerCharacter()` method
- **Logic**: 
  - Health decreases over time based on current health percentage
  - More health = faster decay ("Comfort makes us weak")
  - Configurable: base interval (30s), min interval (10s), damage range (1-10)

#### 2. Sanity Manager (`th/sanity/SanityManager.cpp`)
- **Memory Access**: 
  - Griffa value at hardcoded address `0x140000000 + 0x17FE7E0`
  - Health pointer via `player->GetHealth()` at offset `0x184`
- **Hooks**: Multiple game state checks and timers
- **Logic**:
  - Sanity increases when collecting scrap (+10 per 100 scrap)
  - Sanity decreases when taking damage (proportional to damage and health percent)
  - Visual flashback system triggers based on sanity percentage thresholds
  - Director system plays ambient sounds based on sanity level

#### 3. Fuel Manager (`th/fuel/FuelManager.cpp` + `FuelInlineHook.cpp`)
- **Inline Hook**: Patches `movss [rbx+0x0C], xmm0` instruction in fuel consumption
- **Memory Access**: 
  - Fuel pointer at `rbx+0x0C` (vehicle data structure)
  - Vehicle base address tracked via `g_var_vehicleBase`
- **Logic**: Drains fuel when vehicle takes damage or during loading

#### 4. Scrap Manager (`th/scrap/ScrapInlineHook.cpp`)
- **Inline Hook**: Patches collision/loot detection code
- **Memory Access**:
  - Scrap value at `g_var_scraps + 0x24`
  - Canteen value at `g_var_canteen + 0x24`
- **Logic**: Tracks scrap pickup and canteen consumption

#### 5. Vehicle Damage Proxy (`th/vehicle/VehicleDamageProxy.cpp`)
- **Memory Access**: Vehicle health/integrity tracking
- **Logic**: Links vehicle damage to player health and fuel loss

## Configuration System (`th/Config.h` + `etc/th/config.ini`)
- INI-based configuration loaded at plugin attach
- Sections: `decay`, `sanity`, `roguelite`
- All runtime-modifiable without restart

## Memory Safety & Threading
- **Threading**: Main hook runs in detached thread; config loading in main thread
- **Atomic Operations**: Uses `std::atomic` for cross-thread pointer synchronization
- **Memory Protection**: No heap allocations in hot paths; static instances only
- **Cache Management**: Proper `FlushInstructionCache` after code patching

## Key Technical Insights

### Inline Hooking Technique
1. Pattern scan for specific instruction sequences
2. Allocate executable cave memory near target
3. Build trampoline stub with register preservation
4. Redirect execution with relative JMP
5. Restore original instructions in trampoline

### Direct Memory Access
- Hardcoded offsets used for performance
- No virtual function calls in hot paths
- Pointer arithmetic instead of vtable lookups

### Game State Integration
- Checks `CGameState` for menu/loading states
- Integrates with GTA:SA's existing entity system
- Minimal overhead per-frame calculation

## Roguelite Features
- Scrap loss on death
- Fuel loss on car damage
- Empty canteen on loading screen
- All tied to memory variables for persistence

## Visual/UI System (`th/sanity/SanityHUD.cpp`)
- Custom HUD rendering via IMGUI integration
- Texture streaming for flashback images
- Dynamic text and sound triggering
- Configurable visibility thresholds

## Security & Anti-Cheat Considerations
- No external memory reads/writes (pure inline hooks)
- No signature scanning at runtime (hardcoded addresses)
- Minimal API footprint
- Could be flagged by heuristic anti-cheat due to code injection

## Performance Characteristics
- Per-frame cost: ~5-10 microseconds (estimated)
- Memory footprint: ~2MB additional (including IMGUI)
- No garbage collection or dynamic alloc in gameplay loop

## Known Limitations
- Hardcoded addresses may break across game versions/patches
- No error recovery if game structure changes
- Thread synchronization minimal (relies on atomic pointers)
