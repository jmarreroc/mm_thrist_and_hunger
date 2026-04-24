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