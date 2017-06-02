
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
#define ID_FUNCTION_RESET             0xa000
#define ID_LOCATION_a041              0xa041
#define ID_LOCATION_a002              0xa002
#define ID_LOCATION_a106              0xa106
#define ID_LOCATION_a447              0xa447
#define ID_LOCATION_a049              0xa049
#define ID_LOCATION_a14a              0xa14a
#define ID_LOCATION_a137              0xa137
#define ID_LOCATION_a14c              0xa14c
#define ID_LOCATION_a44e              0xa44e
#define ID_LOCATION_a056              0xa056
#define ID_LOCATION_a097              0xa097
#define ID_FUNCTION_NMI               0xa45a
#define ID_LOCATION_a01b              0xa01b
#define ID_LOCATION_a0dc              0xa0dc
#define ID_LOCATION_a0a1              0xa0a1
#define ID_LOCATION_a0e3              0xa0e3
#define ID_LOCATION_a166              0xa166
#define ID_FUNCTION_IRQ               0xa46b
#define ID_LOCATION_a06d              0xa06d
#define ID_LOCATION_a130              0xa130
#define ID_LOCATION_a032              0xa032
#define ID_LOCATION_a0b6              0xa0b6
#define ID_LOCATION_a077              0xa077
#define ID_LOCATION_a43e              0xa43e

        
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
