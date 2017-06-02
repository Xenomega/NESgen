
#ifndef GAME_H_
#define GAME_H_
#include "NESsys.h"
#include "memory.h"
#include "instructions.h"
#include "ppu.h"

// ---------------------------------
// Objects/Structures
// ---------------------------------
enum MIRRORINGTYPE mirroringType;
extern struct TLBEntry gameTLB[];
UINT gameTLBSize;

// ---------------------------------
// Function IDs (used for interrupt/JSR locations).
// ---------------------------------
#define ID_FUNCTION_RESET             0x8000
#define ID_LOCATION_8041              0x8041
#define ID_LOCATION_8002              0x8002
#define ID_LOCATION_809b              0x809b
#define ID_LOCATION_80ab              0x80ab
#define ID_LOCATION_8055              0x8055
#define ID_LOCATION_808a              0x808a
#define ID_LOCATION_801b              0x801b
#define ID_LOCATION_8032              0x8032
#define ID_FUNCTION_NMI               0x84ae
#define ID_FUNCTION_IRQ               0x84af

        
// ---------------------------------
// Function Mappings
// ---------------------------------

#define sync(interval)            if(cpu_sync(interval)) { return TRUE; }

// ---------------------------------
// Functions
// ---------------------------------
BOOL game_execute(USHORT jumpAddress);

// ---------------------------------
// Data
// ---------------------------------
extern BYTE prgRomData[];
UINT prgRomDataSize;
extern BYTE chrRom[];
UINT chrRomSize;

#endif /* GAME_H_ */
