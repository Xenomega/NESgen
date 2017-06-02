
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
#define ID_LOCATION_8080              0x8080
#define ID_LOCATION_8002              0x8002
#define ID_LOCATION_81a3              0x81a3
#define ID_LOCATION_80b1              0x80b1
#define ID_LOCATION_804b              0x804b
#define ID_LOCATION_804d              0x804d
#define ID_LOCATION_806e              0x806e
#define ID_LOCATION_8211              0x8211
#define ID_FUNCTION_IRQ               0x8233
#define ID_LOCATION_8194              0x8194
#define ID_LOCATION_8056              0x8056
#define ID_LOCATION_821f              0x821f
#define ID_LOCATION_801b              0x801b
#define ID_FUNCTION_NMI               0x81eb

        
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
