
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
#define ID_LOCATION_8102              0x8102
#define ID_LOCATION_8143              0x8143
#define ID_LOCATION_81ca              0x81ca
#define ID_FUNCTION_IRQ               0x824d
#define ID_LOCATION_818e              0x818e
#define ID_LOCATION_8194              0x8194
#define ID_LOCATION_8215              0x8215
#define ID_FUNCTION_RESET             0x80d7
#define ID_LOCATION_8198              0x8198
#define ID_LOCATION_8219              0x8219
#define ID_LOCATION_819b              0x819b
#define ID_LOCATION_823a              0x823a
#define ID_LOCATION_819e              0x819e
#define ID_LOCATION_815f              0x815f
#define ID_LOCATION_80e3              0x80e3
#define ID_LOCATION_8124              0x8124
#define ID_LOCATION_81a5              0x81a5
#define ID_LOCATION_814e              0x814e
#define ID_LOCATION_816b              0x816b
#define ID_LOCATION_822e              0x822e
#define ID_LOCATION_81b3              0x81b3
#define ID_FUNCTION_NMI               0x81f4
#define ID_LOCATION_80de              0x80de
#define ID_LOCATION_80d9              0x80d9
#define ID_LOCATION_817b              0x817b

        
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
