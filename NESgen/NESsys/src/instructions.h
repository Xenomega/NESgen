/*
 * instructions.h
 *
 *  Created on: Jan 19, 2017
 *      Author: Vyper
 */

#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_
#include "cpu.h"
#include "NESsys.h"
#include "memory.h"

// ---------------------------------
// Functions
// ---------------------------------
void MOSInstr_ADC(BYTE value);
void MOSInstr_AND(BYTE value);
BYTE MOSInstr_ASL(BYTE value);
void MOSInstr_BIT(BYTE value);
void MOSInstr_BRK();
void MOSInstr_CLC();
void MOSInstr_CLD();
void MOSInstr_CLI();
void MOSInstr_CLV();
void MOSInstr_CMP(BYTE value);
void MOSInstr_CPX(BYTE value);
void MOSInstr_CPY(BYTE value);
BYTE MOSInstr_DEC(BYTE value);
void MOSInstr_DEX();
void MOSInstr_DEY();
void MOSInstr_EOR(BYTE value);
BYTE MOSInstr_INC(BYTE value);
void MOSInstr_INX();
void MOSInstr_INY();
void MOSInstr_LDA(BYTE value);
void MOSInstr_LDX(BYTE value);
void MOSInstr_LDY(BYTE value);
BYTE MOSInstr_LSR(BYTE value);
void MOSInstr_NOP();
void MOSInstr_ORA(BYTE value);
void MOSInstr_PHA();
void MOSInstr_PHP();
void MOSInstr_PLA();
void MOSInstr_PLP();
BYTE MOSInstr_ROL(BYTE value);
BYTE MOSInstr_ROR(BYTE value);
void MOSInstr_RTI();
void MOSInstr_SBC(BYTE value);
void MOSInstr_SEC();
void MOSInstr_SED();
void MOSInstr_SEI();
void MOSInstr_STA(USHORT addr);
void MOSInstr_STX(USHORT addr);
void MOSInstr_STY(USHORT addr);
void MOSInstr_TAX();
void MOSInstr_TAY();
void MOSInstr_TSX();
void MOSInstr_TXA();
void MOSInstr_TXS();
void MOSInstr_TYA();

#endif /* INSTRUCTIONS_H_ */
