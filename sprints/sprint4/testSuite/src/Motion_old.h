
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
#define ID_LOCATION_8002              0x8002
#define ID_FUNCTION_NMI               0xf8e5
#define ID_LOCATION_f8e4              0xf8e4
#define ID_LOCATION_f8c5              0xf8c5
#define ID_LOCATION_8094              0x8094
#define ID_LOCATION_f8f4              0xf8f4
#define ID_LOCATION_8036              0x8036
#define ID_LOCATION_f897              0xf897
#define ID_FUNCTION_IRQ               0xf91b
#define ID_LOCATION_8059              0x8059
#define ID_LOCATION_801b              0x801b
#define ID_LOCATION_8045              0x8045

        
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
