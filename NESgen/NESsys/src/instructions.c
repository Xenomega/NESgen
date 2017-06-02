#include "game_base.h"
#include "instructions.h"

/*
 * NOTES:
 * -BCC,BCS,BEQ,BMI,BNE,BPL,BVC,BVS,JMP,JSR,RTI,RTS are all replaced with C style control flow.
 */

/*
 * Adds given value to accumulator (and adds carry bit).
 */
void MOSInstr_ADC(BYTE value)
{
	// TODO: Revisit to verify this.
	USHORT result = registers.A + value + cpu_get_flag(CPU_FLAG_CARRY);
	cpu_set_flag(CPU_FLAG_CARRY, result > 0xFF); // set if result carried over 8-bit
	result &= 0xFF; // ensure 8-bit from now on
	cpu_set_flag(CPU_FLAG_ZERO, result == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, result & 0x80); // set if sign bit is set
	cpu_set_flag(CPU_FLAG_OVERFLOW, ~(registers.A ^ value) & 0x80 & (registers.A ^ result)); // set if operands results were same but result's wasn't.
	registers.A = (BYTE)result;
}
/*
 * ANDs the accumulator with the given value.
 */
void MOSInstr_AND(BYTE value)
{
	registers.A &= value;
	cpu_set_flag(CPU_FLAG_ZERO, registers.A == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.A & 0x80); // set if sign bit is set
}
/*
 * Shifts the given value one bit to the left, and returns it.
 * (meant to be put in the accumulator or memory depending on addressing mode)
 */
BYTE MOSInstr_ASL(BYTE value)
{
	cpu_set_flag(CPU_FLAG_CARRY, value & 0x80); // set if result will be carried over 8-bit
	value <<= 1; // shift one bit to the left
	cpu_set_flag(CPU_FLAG_ZERO, value == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, value & 0x80); // set if sign bit is set
	return value;
}
/*
 * Tests many different bits on the given value.
 */
void MOSInstr_BIT(BYTE value)
{
	cpu_set_flag(CPU_FLAG_OVERFLOW, value & 0x40); // copy bit 6 to overflow.
	cpu_set_flag(CPU_FLAG_ZERO, (value & registers.A) == 0); // set if value and A don't share any bits
	cpu_set_flag(CPU_FLAG_SIGN, value & 0x80); // set if sign bit is set
}
/*
 * Initiates the IRQ interrupt.
 */
void MOSInstr_BRK()
{
	if(!cpu_get_flag(CPU_FLAG_INTERRUPT_DISABLE))
		interrupts.requestedIRQ = TRUE;
}
/*
 * Clears the carry flag
 */
void MOSInstr_CLC()
{
	cpu_set_flag(CPU_FLAG_CARRY, FALSE);
}
/*
 * Clears the decimal flag (Unused on NES)
 */
void MOSInstr_CLD()
{
	cpu_set_flag(CPU_FLAG_DECIMAL, FALSE);
}
/*
 * Clears the interrupt disable flag
 */
void MOSInstr_CLI()
{
	cpu_set_flag(CPU_FLAG_INTERRUPT_DISABLE, FALSE);
}
/*
 * Clears the overflow flag
 */
void MOSInstr_CLV()
{
	cpu_set_flag(CPU_FLAG_OVERFLOW, FALSE);
}
/*
 * Compares difference of given value and accumulator and stores in flags.
 */
void MOSInstr_CMP(BYTE value)
{
	cpu_set_flag(CPU_FLAG_CARRY, registers.A >= value); // set if A exceeds/matches value
	value = registers.A - value;
	cpu_set_flag(CPU_FLAG_ZERO, value == 0); // set if difference is 0
	cpu_set_flag(CPU_FLAG_SIGN, value & 0x80); // set if sign bit on difference is set
}
/*
 * Compares difference of given value and X register and stores in flags.
 */
void MOSInstr_CPX(BYTE value)
{
	cpu_set_flag(CPU_FLAG_CARRY, registers.X >= value); // set if X exceeds/matches value
	value = registers.X - value;
	cpu_set_flag(CPU_FLAG_ZERO, value == 0); // set if difference is 0
	cpu_set_flag(CPU_FLAG_SIGN, value & 0x80); // set if sign bit on difference is set
}
/*
 * Compares difference of given value and Y register and stores in flags.
 */
void MOSInstr_CPY(BYTE value)
{
	cpu_set_flag(CPU_FLAG_CARRY, registers.Y >= value); // set if Y exceeds/matches value
	value = registers.Y - value;
	cpu_set_flag(CPU_FLAG_ZERO, value == 0); // set if difference is 0
	cpu_set_flag(CPU_FLAG_SIGN, value & 0x80); // set if sign bit on difference is set
}
/*
 * Decrements the given value and returns it.
 * (meant to be put back into the same memory position)
 */
BYTE MOSInstr_DEC(BYTE value)
{
	value--;
	cpu_set_flag(CPU_FLAG_ZERO, value == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, value & 0x80); // set if sign bit is set
	return value;
}
/*
 * Decrements the X register.
 */
void MOSInstr_DEX()
{
	registers.X--;
	cpu_set_flag(CPU_FLAG_ZERO, registers.X == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.X & 0x80); // set if sign bit is set
}
/*
 * Decrements the Y register.
 */
void MOSInstr_DEY()
{
	registers.Y--;
	cpu_set_flag(CPU_FLAG_ZERO, registers.Y == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.Y & 0x80); // set if sign bit is set
}
/*
 * XORs the accumulator with the given value
 */
void MOSInstr_EOR(BYTE value)
{
	registers.A ^= value;
	cpu_set_flag(CPU_FLAG_ZERO, registers.A == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.A & 0x80); // set if sign bit is set
}
/*
 * Increments the given value and returns it.
 * (meant to be put back into the same memory position)
 */
BYTE MOSInstr_INC(BYTE value)
{
	value++;
	cpu_set_flag(CPU_FLAG_ZERO, value == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, value & 0x80); // set if sign bit is set
	return value;
}
/*
 * Increments the X register.
 */
void MOSInstr_INX()
{
	registers.X++;
	cpu_set_flag(CPU_FLAG_ZERO, registers.X == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.X & 0x80); // set if sign bit is set
}
/*
 * Increments the Y register.
 */
void MOSInstr_INY()
{
	registers.Y++;
	cpu_set_flag(CPU_FLAG_ZERO, registers.Y == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.Y & 0x80); // set if sign bit is set
}
/*
 * Loads the accumulator with the given value.
 */
void MOSInstr_LDA(BYTE value)
{
	registers.A = value;
	cpu_set_flag(CPU_FLAG_ZERO, registers.A == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.A & 0x80); // set if sign bit is set
}
/*
 * Loads the X register with the given value.
 */
void MOSInstr_LDX(BYTE value)
{
	registers.X = value;
	cpu_set_flag(CPU_FLAG_ZERO, registers.X == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.X & 0x80); // set if sign bit is set
}
/*
 * Loads the Y register with the given value.
 */
void MOSInstr_LDY(BYTE value)
{
	registers.Y = value;
	cpu_set_flag(CPU_FLAG_ZERO, registers.Y == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.Y & 0x80); // set if sign bit is set
}
/*
 * Shifts the given value one bit to the right, and returns it.
 * (meant to be put in the accumulator or memory depending on addressing mode)
 */
BYTE MOSInstr_LSR(BYTE value)
{
	cpu_set_flag(CPU_FLAG_CARRY, value & 0x01); // set if the right-most bit will exit.
	value >>= 1; // shift one bit to the left
	cpu_set_flag(CPU_FLAG_ZERO, value == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, value & 0x80); // set if sign bit is set
	return value;
}
/*
 * No operation, does nothing.
 */
void MOSInstr_NOP()
{

}
/*
 * ORs the accumulator with the given value
 */
void MOSInstr_ORA(BYTE value)
{
	registers.A |= value;
	cpu_set_flag(CPU_FLAG_ZERO, registers.A == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.A & 0x80); // set if sign bit is set
}
/*
 * Push accumulator to stack
 */
void MOSInstr_PHA()
{
	cpu_stack_push(registers.A);
}
/*
 * Push processor status flags to stack
 */
void MOSInstr_PHP()
{
	cpu_stack_push(registers.P);
}
/*
 * Pull accumulator from stack
 */
void MOSInstr_PLA()
{
	registers.A = cpu_stack_pop();
	cpu_set_flag(CPU_FLAG_ZERO, registers.A == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.A & 0x80); // set if sign bit is set
}
/*
 * Pull processor status flags from stack
 */
void MOSInstr_PLP()
{
	cpu_set_flags(cpu_stack_pop());
}
/*
 * Rotates value left by one bit.
 * (meant to be put in the accumulator or memory depending on addressing mode)
 */
BYTE MOSInstr_ROL(BYTE value)
{
	USHORT result = value << 1;
	result |= cpu_get_flag(CPU_FLAG_CARRY);
	cpu_set_flag(CPU_FLAG_CARRY, result > 0xFF); // set if result carried over 8-bit
	result &= 0xFF; // clear upper bits to ensure 8-bit
	cpu_set_flag(CPU_FLAG_ZERO, result == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, result & 0x80); // set if sign bit is set
	return (BYTE)result;
}
/*
 * Rotates value right by one bit.
 * (meant to be put in the accumulator or memory depending on addressing mode)
 */
BYTE MOSInstr_ROR(BYTE value)
{
	USHORT result = value;
	result |= (cpu_get_flag(CPU_FLAG_CARRY) << 8); // make our 7th bit our carry flag after rotation
	cpu_set_flag(CPU_FLAG_CARRY, result & 0x01); // set if we will carry out of right side
	result >>= 1;
	cpu_set_flag(CPU_FLAG_ZERO, result == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, result & 0x80); // set if sign bit is set
	return (BYTE)result;
}
/*
 * Return from interrupt: pulls stack pointer and program counter (unused) from stack.
 */
void MOSInstr_RTI()
{
	registers.P = cpu_stack_pop();
}
/*
 * Subtract from accumulator (with borrow)
 */
void MOSInstr_SBC(BYTE value)
{
	USHORT result = (registers.A - value) - (1 - cpu_get_flag(CPU_FLAG_CARRY));
	cpu_set_flag(CPU_FLAG_CARRY, result <= 0xFF); // set if result didn't carry over 8-bit.
	result &= 0xFF; // ensure 8-bit from this point forward.
	cpu_set_flag(CPU_FLAG_ZERO, result == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, result & 0x80); // set if sign bit is set
	cpu_set_flag(CPU_FLAG_OVERFLOW, (registers.A ^ value) & 0x80 & (registers.A ^ result)); // set if A has different sign than memory and result

	registers.A = (BYTE)result;
}
/*
 * Sets the carry flag
 */
void MOSInstr_SEC()
{
	cpu_set_flag(CPU_FLAG_CARRY, TRUE);
}
/*
 * Sets the decimal flag (Unused on NES)
 */
void MOSInstr_SED()
{
	cpu_set_flag(CPU_FLAG_DECIMAL, TRUE);
}
/*
 * Sets the interrupt disable flag
 */
void MOSInstr_SEI()
{
	cpu_set_flag(CPU_FLAG_INTERRUPT_DISABLE, TRUE);
}
/*
 * Store the accumulator at the given address
 */
void MOSInstr_STA(USHORT addr)
{
	cpu_write8(addr, registers.A);
}
/*
 * Store the X register at the given address
 */
void MOSInstr_STX(USHORT addr)
{
	cpu_write8(addr, registers.X);
}
/*
 * Store the Y register at the given address
 */
void MOSInstr_STY(USHORT addr)
{
	cpu_write8(addr, registers.Y);
}
/*
 * Transfer accumulator to X register
 */
void MOSInstr_TAX()
{
	registers.X = registers.A;
	cpu_set_flag(CPU_FLAG_ZERO, registers.X == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.X & 0x80); // set if sign bit is set
}
/*
 * Transfer accumulator to Y register
 */
void MOSInstr_TAY()
{
	registers.Y = registers.A;
	cpu_set_flag(CPU_FLAG_ZERO, registers.Y == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.Y & 0x80); // set if sign bit is set
}
/*
 * Transfer stack pointer to X register.
 */
void MOSInstr_TSX()
{
	registers.X = registers.SP;
	cpu_set_flag(CPU_FLAG_ZERO, registers.X == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.X & 0x80); // set if sign bit is set
}
/*
 * Transfer X register to accumulator.
 */
void MOSInstr_TXA()
{
	registers.A = registers.X;
	cpu_set_flag(CPU_FLAG_ZERO, registers.A == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.A & 0x80); // set if sign bit is set
}
/*
 * Transfer X register to stack pointer.
 */
void MOSInstr_TXS()
{
	registers.SP = registers.X;
}
/*
 * Transfer Y register to accumulator.
 */
void MOSInstr_TYA()
{
	registers.A = registers.Y;
	cpu_set_flag(CPU_FLAG_ZERO, registers.A == 0); // set if result is 0
	cpu_set_flag(CPU_FLAG_SIGN, registers.A & 0x80); // set if sign bit is set
}
