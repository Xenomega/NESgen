
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
extern BYTE DATA_f91c[];
extern BYTE DATA_8097[];

extern BYTE chrRom[];
UINT chrRomSize;

#endif /* GAME_H_ */
