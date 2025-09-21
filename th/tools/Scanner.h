#include <cstdint>
#include <cstddef>
#include <mm/game/go/character.h>
#include <mm/game/go/vehicle.h>

class CCharacter;

void ScanVehicleFields(CVehicle* vehicle, size_t maxOffset = 0x400);
void ScanCharacterFields(CCharacter* player, size_t maxOffset = 0x400);
void ScanAll();
void ScanVehicleForFuel(CCharacter* vehicle);
