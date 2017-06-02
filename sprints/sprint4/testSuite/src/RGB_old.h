
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
#define ID_LOCATION_8103              0x8103
#define ID_LOCATION_8405              0x8405
#define ID_LOCATION_8787              0x8787
#define ID_LOCATION_860b              0x860b
#define ID_LOCATION_810d              0x810d
#define ID_LOCATION_80ef              0x80ef
#define ID_LOCATION_8310              0x8310
#define ID_LOCATION_8491              0x8491
#define ID_LOCATION_8683              0x8683
#define ID_LOCATION_8117              0x8117
#define ID_LOCATION_821b              0x821b
#define ID_LOCATION_851d              0x851d
#define ID_LOCATION_861f              0x861f
#define ID_LOCATION_8121              0x8121
#define ID_LOCATION_870f              0x870f
#define ID_LOCATION_801b              0x801b
#define ID_LOCATION_88a7              0x88a7
#define ID_LOCATION_87a8              0x87a8
#define ID_LOCATION_86ab              0x86ab
#define ID_LOCATION_882d              0x882d
#define ID_LOCATION_829d              0x829d
#define ID_LOCATION_88b1              0x88b1
#define ID_LOCATION_8032              0x8032
#define ID_LOCATION_8633              0x8633
#define ID_LOCATION_88b5              0x88b5
#define ID_LOCATION_8737              0x8737
#define ID_LOCATION_88b9              0x88b9
#define ID_LOCATION_85bb              0x85bb
#define ID_LOCATION_86bf              0x86bf
#define ID_LOCATION_8811              0x8811
#define ID_LOCATION_8647              0x8647
#define ID_LOCATION_8773              0x8773
#define ID_LOCATION_874b              0x874b
#define ID_LOCATION_824d              0x824d
#define ID_LOCATION_85e3              0x85e3
#define ID_LOCATION_8351              0x8351
#define ID_LOCATION_88e3              0x88e3
#define ID_LOCATION_8723              0x8723
#define ID_LOCATION_865b              0x865b
#define ID_LOCATION_885c              0x885c
#define ID_LOCATION_81df              0x81df
#define ID_LOCATION_875f              0x875f
#define ID_LOCATION_82e3              0x82e3
#define ID_LOCATION_86e7              0x86e7
#define ID_FUNCTION_NMI               0x885f
#define ID_LOCATION_8697              0x8697
#define ID_LOCATION_866f              0x866f
#define ID_LOCATION_881b              0x881b
#define ID_FUNCTION_IRQ               0x88f1
#define ID_LOCATION_86d3              0x86d3
#define ID_LOCATION_8874              0x8874
#define ID_LOCATION_85cf              0x85cf
#define ID_LOCATION_85f7              0x85f7
#define ID_LOCATION_80f9              0x80f9
#define ID_LOCATION_86fb              0x86fb
#define ID_LOCATION_81fd              0x81fd
#define ID_LOCATION_837e              0x837e

        
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
