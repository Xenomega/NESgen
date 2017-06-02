
#ifndef GAME_BASE_H_
#define GAME_BASE_H_
#include "NESsys.h"
#include "memory.h"
#include "instructions.h"
#include "ppu.h"

#if APPLICATION_USE_TEMPLATE
// ---------------------------------
// Data Segment Lookup Table
// ---------------------------------
enum MIRRORINGTYPE mirroringType;
extern struct TLBEntry gameTLB[];
UINT gameTLBSize;

// ---------------------------------
// Function IDs (used for interrupt/JSR locations).
// ---------------------------------
#define ID_FUNCTION_RESET			0
#define ID_FUNCTION_NMI				1
#define ID_FUNCTION_IRQ				2


// ---------------------------------
// Functions
// ---------------------------------
#define SYNC(interval)            if(cpu_sync(interval)) { return TRUE; }
BOOL game_execute(USHORT jumpAddress);

// ---------------------------------
// Data
// ---------------------------------
extern BYTE prgRomData[];
UINT prgRomDataSize;
extern BYTE chrRom[];
UINT chrRomSize;
#else
#include "game.h"
#endif

#endif /* GAME_BASE_H_ */
