
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
#define ID_LOCATION_a441              0xa441
#define ID_LOCATION_a049              0xa049
#define ID_LOCATION_a44a              0xa44a
#define ID_LOCATION_a137              0xa137
#define ID_LOCATION_a14c              0xa14c
#define ID_LOCATION_a451              0xa451
#define ID_LOCATION_a056              0xa056
#define ID_LOCATION_a097              0xa097
#define ID_LOCATION_a01b              0xa01b
#define ID_LOCATION_a0dc              0xa0dc
#define ID_FUNCTION_NMI               0xa45d
#define ID_LOCATION_a0a1              0xa0a1
#define ID_LOCATION_a0e3              0xa0e3
#define ID_LOCATION_a169              0xa169
#define ID_LOCATION_a06d              0xa06d
#define ID_LOCATION_a130              0xa130
#define ID_LOCATION_a032              0xa032
#define ID_FUNCTION_IRQ               0xa469
#define ID_LOCATION_a0b6              0xa0b6
#define ID_LOCATION_a077              0xa077
#define ID_LOCATION_a14a              0xa14a

        
// ---------------------------------
// Function Mappings
// ---------------------------------
#define RequestBRK                    MOSInstr_BRK
#define OrA                           MOSInstr_ORA
#define OrA                           MOSInstr_ORA
#define ShiftLeft                     MOSInstr_ASL
#define PushFlags                     MOSInstr_PHP
#define OrA                           MOSInstr_ORA
#define ShiftLeft                     MOSInstr_ASL
#define OrA                           MOSInstr_ORA
#define ShiftLeft                     MOSInstr_ASL
#define OrA                           MOSInstr_ORA
#define OrA                           MOSInstr_ORA
#define ShiftLeft                     MOSInstr_ASL
#define ClearCarryFlag                MOSInstr_CLC
#define OrA                           MOSInstr_ORA
#define OrA                           MOSInstr_ORA
#define ShiftLeft                     MOSInstr_ASL
#define AndA                          MOSInstr_AND
#define TEST                          MOSInstr_BIT
#define AndA                          MOSInstr_AND
#define RotateLeft                    MOSInstr_ROL
#define PopFlags                      MOSInstr_PLP
#define AndA                          MOSInstr_AND
#define RotateLeft                    MOSInstr_ROL
#define TEST                          MOSInstr_BIT
#define AndA                          MOSInstr_AND
#define RotateLeft                    MOSInstr_ROL
#define AndA                          MOSInstr_AND
#define AndA                          MOSInstr_AND
#define RotateLeft                    MOSInstr_ROL
#define SetCarryFlag                  MOSInstr_SEC
#define AndA                          MOSInstr_AND
#define AndA                          MOSInstr_AND
#define RotateLeft                    MOSInstr_ROL
#define XORWithA                      MOSInstr_EOR
#define XORWithA                      MOSInstr_EOR
#define ShiftRight                    MOSInstr_LSR
#define PushA                         MOSInstr_PHA
#define XORWithA                      MOSInstr_EOR
#define ShiftRight                    MOSInstr_LSR
#define XORWithA                      MOSInstr_EOR
#define ShiftRight                    MOSInstr_LSR
#define XORWithA                      MOSInstr_EOR
#define XORWithA                      MOSInstr_EOR
#define ShiftRight                    MOSInstr_LSR
#define ClearInterruptDisableFlag     MOSInstr_CLI
#define XORWithA                      MOSInstr_EOR
#define XORWithA                      MOSInstr_EOR
#define ShiftRight                    MOSInstr_LSR
#define AddToA                        MOSInstr_ADC
#define AddToA                        MOSInstr_ADC
#define RotateRight                   MOSInstr_ROR
#define PopA                          MOSInstr_PLA
#define AddToA                        MOSInstr_ADC
#define RotateRight                   MOSInstr_ROR
#define AddToA                        MOSInstr_ADC
#define RotateRight                   MOSInstr_ROR
#define AddToA                        MOSInstr_ADC
#define AddToA                        MOSInstr_ADC
#define RotateRight                   MOSInstr_ROR
#define SetInterruptDisableFlag       MOSInstr_SEI
#define AddToA                        MOSInstr_ADC
#define AddToA                        MOSInstr_ADC
#define RotateRight                   MOSInstr_ROR
#define StoreA                        MOSInstr_STA
#define StoreY                        MOSInstr_STY
#define StoreA                        MOSInstr_STA
#define StoreX                        MOSInstr_STX
#define DecrementY                    MOSInstr_DEY
#define MoveXToA                      MOSInstr_TXA
#define StoreY                        MOSInstr_STY
#define StoreA                        MOSInstr_STA
#define StoreX                        MOSInstr_STX
#define StoreA                        MOSInstr_STA
#define StoreY                        MOSInstr_STY
#define StoreA                        MOSInstr_STA
#define StoreX                        MOSInstr_STX
#define MoveYToA                      MOSInstr_TYA
#define StoreA                        MOSInstr_STA
#define MoveXToSP                     MOSInstr_TXS
#define StoreA                        MOSInstr_STA
#define SetY                          MOSInstr_LDY
#define SetA                          MOSInstr_LDA
#define SetX                          MOSInstr_LDX
#define SetY                          MOSInstr_LDY
#define SetA                          MOSInstr_LDA
#define SetX                          MOSInstr_LDX
#define MoveAToY                      MOSInstr_TAY
#define SetA                          MOSInstr_LDA
#define MoveAToX                      MOSInstr_TAX
#define SetY                          MOSInstr_LDY
#define SetA                          MOSInstr_LDA
#define SetX                          MOSInstr_LDX
#define SetA                          MOSInstr_LDA
#define SetY                          MOSInstr_LDY
#define SetA                          MOSInstr_LDA
#define SetX                          MOSInstr_LDX
#define ClearOverflowFlag             MOSInstr_CLV
#define SetA                          MOSInstr_LDA
#define MoveSPToX                     MOSInstr_TSX
#define SetY                          MOSInstr_LDY
#define SetA                          MOSInstr_LDA
#define SetX                          MOSInstr_LDX
#define CompareWithY                  MOSInstr_CPY
#define CompareWithA                  MOSInstr_CMP
#define CompareWithY                  MOSInstr_CPY
#define CompareWithA                  MOSInstr_CMP
#define Decrement                     MOSInstr_DEC
#define IncrementY                    MOSInstr_INY
#define CompareWithA                  MOSInstr_CMP
#define DecrementX                    MOSInstr_DEX
#define CompareWithY                  MOSInstr_CPY
#define CompareWithA                  MOSInstr_CMP
#define Decrement                     MOSInstr_DEC
#define CompareWithA                  MOSInstr_CMP
#define CompareWithA                  MOSInstr_CMP
#define Decrement                     MOSInstr_DEC
#define ClearDecimalFlag              MOSInstr_CLD
#define CompareWithA                  MOSInstr_CMP
#define CompareWithA                  MOSInstr_CMP
#define Decrement                     MOSInstr_DEC
#define CompareWithX                  MOSInstr_CPX
#define SubtractFromA                 MOSInstr_SBC
#define CompareWithX                  MOSInstr_CPX
#define SubtractFromA                 MOSInstr_SBC
#define Increment                     MOSInstr_INC
#define IncrementX                    MOSInstr_INX
#define SubtractFromA                 MOSInstr_SBC
#define CompareWithX                  MOSInstr_CPX
#define SubtractFromA                 MOSInstr_SBC
#define Increment                     MOSInstr_INC
#define SubtractFromA                 MOSInstr_SBC
#define SubtractFromA                 MOSInstr_SBC
#define Increment                     MOSInstr_INC
#define SetDecimalFlag                MOSInstr_SED
#define SubtractFromA                 MOSInstr_SBC
#define SubtractFromA                 MOSInstr_SBC
#define Increment                     MOSInstr_INC

#define sync(interval)            if(cpu_sync(interval)) { return TRUE; }

// ---------------------------------
// Functions
// ---------------------------------
BOOL game_execute(UINT functionID);

// ---------------------------------
// Data
// ---------------------------------
extern BYTE DATA_8000[];
extern BYTE DATA_a171[];
extern BYTE DATA_a46a[];

extern BYTE chrRom[];
UINT chrRomSize;

#endif /* GAME_H_ */
