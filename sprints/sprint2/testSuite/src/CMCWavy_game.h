
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
#define ID_LOCATION_8041              0x8041
#define ID_LOCATION_8002              0x8002
#define ID_LOCATION_8903              0x8903
#define ID_LOCATION_8804              0x8804
#define ID_LOCATION_8885              0x8885
#define ID_LOCATION_8906              0x8906
#define ID_LOCATION_8181              0x8181
#define ID_LOCATION_8908              0x8908
#define ID_LOCATION_88c9              0x88c9
#define ID_LOCATION_8086              0x8086
#define ID_LOCATION_8897              0x8897
#define ID_LOCATION_889e              0x889e
#define ID_LOCATION_88d0              0x88d0
#define ID_LOCATION_8193              0x8193
#define ID_LOCATION_8055              0x8055
#define ID_LOCATION_8957              0x8957
#define ID_FUNCTION_IRQ               0x898a
#define ID_LOCATION_891a              0x891a
#define ID_LOCATION_88e9              0x88e9
#define ID_LOCATION_8075              0x8075
#define ID_LOCATION_809a              0x809a
#define ID_LOCATION_895e              0x895e
#define ID_LOCATION_8960              0x8960
#define ID_LOCATION_88e2              0x88e2
#define ID_LOCATION_801b              0x801b
#define ID_LOCATION_8865              0x8865
#define ID_LOCATION_8186              0x8186
#define ID_FUNCTION_NMI               0x87e9
#define ID_LOCATION_886c              0x886c
#define ID_LOCATION_88b0              0x88b0
#define ID_LOCATION_8988              0x8988
#define ID_LOCATION_8032              0x8032
#define ID_LOCATION_87f5              0x87f5
#define ID_LOCATION_8936              0x8936
#define ID_LOCATION_88b7              0x88b7
#define ID_LOCATION_8838              0x8838
#define ID_LOCATION_81b7              0x81b7
#define ID_LOCATION_883f              0x883f
#define ID_LOCATION_817d              0x817d
#define ID_LOCATION_887e              0x887e
#define ID_LOCATION_88ff              0x88ff

        
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
extern BYTE DATA_81c9[];
extern BYTE DATA_898b[];

extern BYTE chrRom[];
UINT chrRomSize;

#endif /* GAME_H_ */
