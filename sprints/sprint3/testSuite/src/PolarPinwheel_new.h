
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
#define ID_LOCATION_a443              0xa443
#define ID_LOCATION_a056              0xa056
#define ID_LOCATION_a146              0xa146
#define ID_LOCATION_a148              0xa148
#define ID_LOCATION_a049              0xa049
#define ID_LOCATION_a44a              0xa44a
#define ID_LOCATION_a102              0xa102
#define ID_LOCATION_a093              0xa093
#define ID_FUNCTION_NMI               0xa456
#define ID_LOCATION_a0d8              0xa0d8
#define ID_LOCATION_a01b              0xa01b
#define ID_LOCATION_a09d              0xa09d
#define ID_LOCATION_a0df              0xa0df
#define ID_LOCATION_a162              0xa162
#define ID_LOCATION_a032              0xa032
#define ID_FUNCTION_IRQ               0xa462
#define ID_LOCATION_a12c              0xa12c
#define ID_LOCATION_a06d              0xa06d
#define ID_LOCATION_a0b2              0xa0b2
#define ID_LOCATION_a133              0xa133
#define ID_LOCATION_a077              0xa077
#define ID_LOCATION_a43a              0xa43a

        
// ---------------------------------
// Function Mappings
// ---------------------------------
#define RequestBRK                    MOSInstr_BRK
#define OrA                           MOSInstr_ORA
#define ShiftLeft                     MOSInstr_ASL
#define PushFlags                     MOSInstr_PHP
#define ClearCarryFlag                MOSInstr_CLC
#define AndA                          MOSInstr_AND
#define TEST                          MOSInstr_BIT
#define RotateLeft                    MOSInstr_ROL
#define PopFlags                      MOSInstr_PLP
#define SetCarryFlag                  MOSInstr_SEC
#define XORWithA                      MOSInstr_EOR
#define ShiftRight                    MOSInstr_LSR
#define PushA                         MOSInstr_PHA
#define ClearInterruptDisableFlag     MOSInstr_CLI
#define AddToA                        MOSInstr_ADC
#define RotateRight                   MOSInstr_ROR
#define PopA                          MOSInstr_PLA
#define SetInterruptDisableFlag       MOSInstr_SEI
#define StoreA                        MOSInstr_STA
#define StoreY                        MOSInstr_STY
#define StoreX                        MOSInstr_STX
#define DecrementY                    MOSInstr_DEY
#define MoveXToA                      MOSInstr_TXA
#define MoveYToA                      MOSInstr_TYA
#define MoveXToSP                     MOSInstr_TXS
#define SetY                          MOSInstr_LDY
#define SetA                          MOSInstr_LDA
#define SetX                          MOSInstr_LDX
#define MoveAToY                      MOSInstr_TAY
#define MoveAToX                      MOSInstr_TAX
#define ClearOverflowFlag             MOSInstr_CLV
#define MoveSPToX                     MOSInstr_TSX
#define CompareWithY                  MOSInstr_CPY
#define CompareWithA                  MOSInstr_CMP
#define Decrement                     MOSInstr_DEC
#define IncrementY                    MOSInstr_INY
#define DecrementX                    MOSInstr_DEX
#define ClearDecimalFlag              MOSInstr_CLD
#define CompareWithX                  MOSInstr_CPX
#define SubtractFromA                 MOSInstr_SBC
#define Increment                     MOSInstr_INC
#define IncrementX                    MOSInstr_INX
#define SetDecimalFlag                MOSInstr_SED

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
