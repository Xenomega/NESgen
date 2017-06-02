
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
#define ID_FUNCTION_NMI               0x8621
#define ID_FUNCTION_IRQ               0x862a
#define ID_LOCATION_8069              0x8069
#define ID_LOCATION_80ea              0x80ea
#define ID_FUNCTION_RESET             0x8037
#define ID_LOCATION_80ad              0x80ad
#define ID_LOCATION_80fe              0x80fe
#define ID_LOCATION_8052              0x8052
#define ID_LOCATION_8095              0x8095
#define ID_LOCATION_80c9              0x80c9
#define ID_LOCATION_8039              0x8039
#define ID_LOCATION_80bb              0x80bb
#define ID_LOCATION_807d              0x807d
#define ID_LOCATION_80d7              0x80d7

        
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
