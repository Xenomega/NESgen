# MOS-6502 Architecture Instruction Set
# Reference: http://nesdev.com/6502.txt (NOTE: SOME OPCODES ARE INCORRECT)
# Reference: http://www.thealmightyguru.com/Games/Hacking/Wiki/index.php/6502_Opcodes
from enum import Enum
import struct
from NESMemory import NESMemory
from dis import Instruction
class MOSAddressingMode(Enum):
    """The way in which operands are treated in the encapsulating instruction."""
    ACCUMULATOR = 0
    IMMEDIATE = 1
    ABSOLUTE = 2
    ABSOLUTE_X = 3 
    ABSOLUTE_Y = 4
    ZERO_PAGE = 5
    ZERO_PAGE_X = 6
    ZERO_PAGE_Y = 7
    INDIRECT = 8
    INDIRECT_X = 9 # "Pre-indexed indirect"
    INDIRECT_Y = 10 # "Post-indexed indirect"
    IMPLIED = 11
    RELATIVE = 12

class MOSRegisterType(Enum):
    """An enumeration of the valid registers (or subcomponents) in the architecture."""
    A = 0 # Accumulator
    X = 1 # X Index Register
    Y = 2 # Y Index Register
    P = 3 # Status Flags
    SP = 4 # Stack Pointer
    PC = 5 # Program Counter
    FLAG_CARRY = 6
    FLAG_ZERO = 7 # Set when a zero result occurs.
    FLAG_INTERRUPT = 8 # If set, interrupts are disabled.
    FLAG_DECIMAL_MODE = 9 # If set, add/sub with carry input/output values are treated as BCD.
    FLAG_BRK = 10 # Set when software interrupt (BRK) executed.
    FLAG_OVERFLOW = 11 # Set when operation too large to be represented in byte.
    FLAG_SIGN = 12 # Set when result of operation is negative.
    
# An instance of an instruction.
class MOSInstruction:
    """A runtime representation of an instruction."""
    definition = None
    operand = None
    offset = None
    def __init__(self, data, offset):
        # Obtain the opcode and appropriate definition
        opcode = data[offset]
        self.definition = MOSInstrGet(opcode)
        
        # Set our offset, which points to the instruction.
        self.offset = offset
        self.address = NESMemory.offsetToPointer(offset)
        self.operandIsSigned = False
        
        # All instructions follow a simple size scheme based off mode/size (except relative mode which is 8-bit signed)
        if(self.definition.size == 1):
            self.operand = None
        elif(self.definition.size == 2 and self.definition.mode == MOSAddressingMode.RELATIVE):
            self.operand = data[offset+1] if data[offset+1] <= 127 else -(256-data[offset+1]) # 8-bit signed
        elif(self.definition.size == 2):
            self.operand = data[offset+1] # 8-bit unsigned
        elif(self.definition.size == 3):
            self.operand = (data[offset+2] << 8) | data[offset+1] # 16-bit unsigned, little endian
        else:
            raise ValueError("Invalid addressing mode")
        
    def __str__(self):
        """Returns a ASM-like string representation of the instruction."""
        # Create a display string for the instruction. All numbers shown as hex.
        operandStr = ""
        if(self.operand != None):
            operandUnsigned = self.operand if self.operand >= 0 else 256+self.operand 
            operandStr = "$" + hex(operandUnsigned)[2:]
        if(self.definition.mode == MOSAddressingMode.IMMEDIATE):
            operandStr = "#" + operandStr
        elif(self.definition.mode == MOSAddressingMode.ACCUMULATOR):
            operandStr = "A"
        elif(self.definition.mode == MOSAddressingMode.ZERO_PAGE or self.definition.mode == MOSAddressingMode.ABSOLUTE):
            operandStr = operandStr
        elif(self.definition.mode == MOSAddressingMode.ZERO_PAGE_X or self.definition.mode == MOSAddressingMode.ABSOLUTE_X):
            operandStr = operandStr + ",X"
        elif(self.definition.mode == MOSAddressingMode.ZERO_PAGE_Y or self.definition.mode == MOSAddressingMode.ABSOLUTE_Y):
            operandStr = operandStr + ",Y"
        elif(self.definition.mode == MOSAddressingMode.INDIRECT):
            operandStr = "(" + operandStr + ")"
        elif(self.definition.mode == MOSAddressingMode.INDIRECT_X):
            operandStr = "(" + operandStr + ",X)"
        elif(self.definition.mode == MOSAddressingMode.INDIRECT_Y):
            operandStr = "(" + operandStr + "),Y"
        elif(self.definition.mode == MOSAddressingMode.IMPLIED):
            operandStr = ""
        elif(self.definition.mode == MOSAddressingMode.RELATIVE):
            operandStr = operandStr
        else:
            raise ValueError("Invalid addressing mode")
        return (self.definition.name + " " + operandStr if operandStr != "" else self.definition.name) + " ; " +  self.definition.description
    
    
# Define the instruction types
class MOSInstructionDefinition:
    """Defines specific attributes for a type of a given runtime instruction (one instance per type, used by lookup)."""
    opcode = None
    name = None
    description = None
    mode = None
    size = None
    cycles = None
    addCycleIfCrossed = False
    isJumpOrBranch = False
    marksEndOfSection = False
    functionNameOverride = None
    
    def __init__(self, opcode, mode, cycles, addCycleIfCrossed):
        # self.name and self.description should be implemented by the inherited class.
        self.opcode = opcode
        self.mode = mode
        # Opcode is a single byte, the remainder can be determined from mode.
        # Absolute addresses are 16-bit
        if(mode == MOSAddressingMode.ABSOLUTE or
           mode == MOSAddressingMode.ABSOLUTE_X or
           mode == MOSAddressingMode.ABSOLUTE_Y):
            self.size = 3
        # Opcodes without arguments or using accumulator has no arguments.
        elif(mode == MOSAddressingMode.IMPLIED or
             mode == MOSAddressingMode.ACCUMULATOR):
            self.size = 1
        else:
            # Otherwise other modes such as indirect use 8-bit only.
            self.size = 2
        # Set cycle information which will be compiled into our target as well.
        self.cycles = cycles
        self.addCycleIfCrossed = addCycleIfCrossed
            
        

class MOSInstr_ADC(MOSInstructionDefinition):
    name = "ADC"
    description = "Add Memory to Accumulator with Carry"
    functionNameOverride = "AddToA"

class MOSInstr_AND(MOSInstructionDefinition):
    name = "AND"
    description = "\"AND\" Memory with Accumulator"
    functionNameOverride = "AndA"
        
class MOSInstr_ASL(MOSInstructionDefinition):
    name = "ASL"
    description = "Shift Left One Bit (Memory or Accumulator)"
    functionNameOverride = "ShiftLeft"
        
class MOSInstr_BCC(MOSInstructionDefinition):
    name = "BCC"
    description = "Branch on Carry Clear"
    isJumpOrBranch = True
        
class MOSInstr_BCS(MOSInstructionDefinition):
    name = "BCS"
    description = "Branch on Carry Set"
    isJumpOrBranch = True
        
class MOSInstr_BEQ(MOSInstructionDefinition):
    name = "BEQ"
    description = "Branch on Result Zero"
    isJumpOrBranch = True
        
class MOSInstr_BIT(MOSInstructionDefinition):
    name = "BIT"
    description = "Test Bits in Memory with Accumulator"
    functionNameOverride = "TEST"
        
class MOSInstr_BMI(MOSInstructionDefinition):
    name = "BMI"
    description = "Branch on Result Minus"
    isJumpOrBranch = True
    
class MOSInstr_BNE(MOSInstructionDefinition):
    name = "BNE"
    description = "Branch on Result not Zero"
    isJumpOrBranch = True
    
class MOSInstr_BPL(MOSInstructionDefinition):
    name = "BPL"
    description = "Branch on Result Plus"
    isJumpOrBranch = True
    
class MOSInstr_BRK(MOSInstructionDefinition):
    name = "BRK"
    description = "Force Break"
    functionNameOverride = "RequestBRK"
    
class MOSInstr_BVC(MOSInstructionDefinition):
    name = "BVC"
    description = "Branch on Overflow Clear"
    isJumpOrBranch = True

class MOSInstr_BVS(MOSInstructionDefinition):
    name = "BVS"
    description = "Branch on Overflow Set"
    isJumpOrBranch = True
    
class MOSInstr_CLC(MOSInstructionDefinition):
    name = "CLC"
    description = "Clear Carry Flag"
    functionNameOverride = "ClearCarryFlag"
    
class MOSInstr_CLD(MOSInstructionDefinition):
    name = "CLD"
    description = "Clear Decimal Mode"
    functionNameOverride = "ClearDecimalFlag"
    
class MOSInstr_CLI(MOSInstructionDefinition):
    name = "CLI"
    description = "Clear interrupt Disable Bit"
    functionNameOverride = "ClearInterruptDisableFlag"

class MOSInstr_CLV(MOSInstructionDefinition):
    name = "CLV"
    description = "Clear Overflow Flag"
    functionNameOverride = "ClearOverflowFlag"

class MOSInstr_CMP(MOSInstructionDefinition):
    name = "CMP"
    description = "Compare Memory and Accumulator"
    functionNameOverride = "CompareWithA"
    
class MOSInstr_CPX(MOSInstructionDefinition):
    name = "CPX"
    description = "Compare Memory and Index X"
    functionNameOverride = "CompareWithX"
    
class MOSInstr_CPY(MOSInstructionDefinition):
    name = "CPY"
    description = "Compare Memory and Index Y"
    functionNameOverride = "CompareWithY"
    
class MOSInstr_DEC(MOSInstructionDefinition):
    name = "DEC"
    description = "Decrement Memory by One"
    functionNameOverride = "Decrement"
    
class MOSInstr_DEX(MOSInstructionDefinition):
    name = "DEX"
    description = "Decrement Index X by One"
    functionNameOverride = "DecrementX"
    
class MOSInstr_DEY(MOSInstructionDefinition):
    name = "DEY"
    description = "Decrement Index Y by One"
    functionNameOverride = "DecrementY"
    
class MOSInstr_EOR(MOSInstructionDefinition):
    name = "EOR"
    description = "\"Exclusive-OR\" Memory with Accumulator"
    functionNameOverride = "XORWithA"
    
class MOSInstr_INC(MOSInstructionDefinition):
    name = "INC"
    description = "Increment Memory by One"
    functionNameOverride = "Increment"

class MOSInstr_INX(MOSInstructionDefinition):
    name = "INX"
    description = "Increment Index X by One"
    functionNameOverride = "IncrementX"
    
class MOSInstr_INY(MOSInstructionDefinition):
    name = "INY"
    description = "Increment Index Y by One"
    functionNameOverride = "IncrementY"
    
class MOSInstr_JMP(MOSInstructionDefinition):
    name = "JMP"
    description = "Jump to New Location"
    isJumpOrBranch = True
    marksEndOfSection = True
    
class MOSInstr_JSR(MOSInstructionDefinition):
    name = "JSR"
    description = "Jump to New Location Saving Return Address"
    isJumpOrBranch = True
    
class MOSInstr_LDA(MOSInstructionDefinition):
    name = "LDA"
    description = "Load Accumulator with Memory"
    functionNameOverride = "SetA"
    
class MOSInstr_LDX(MOSInstructionDefinition):
    name = "LDX"
    description = "Load Index X with Memory"
    functionNameOverride = "SetX"
    
class MOSInstr_LDY(MOSInstructionDefinition):
    name = "LDY"
    description = "Load Index Y with Memory"
    functionNameOverride = "SetY"
    
class MOSInstr_LSR(MOSInstructionDefinition):
    name = "LSR"
    description = "Shift Right One Bit (Memory or Accumulator)"
    functionNameOverride = "ShiftRight"
    
class MOSInstr_NOP(MOSInstructionDefinition):
    name = "NOP"
    description = "No Operation"
    
class MOSInstr_ORA(MOSInstructionDefinition):
    name = "ORA"
    description = "\"OR\" Memory with Accumulator"
    functionNameOverride = "OrA"
    
class MOSInstr_PHA(MOSInstructionDefinition):
    name = "PHA"
    description = "Push Accumulator on Stack"
    functionNameOverride = "PushA"
    
class MOSInstr_PHP(MOSInstructionDefinition):
    name = "PHP"
    description = "Push Processor Status on Stack"
    functionNameOverride = "PushFlags"
    
class MOSInstr_PLA(MOSInstructionDefinition):
    name = "PLA"
    description = "Pull Accumulator from Stack"
    functionNameOverride = "PopA"
    
class MOSInstr_PLP(MOSInstructionDefinition):
    name = "PLP"
    description = "Pull Processor Status from Stack"
    functionNameOverride = "PopFlags"
    
class MOSInstr_ROL(MOSInstructionDefinition):
    name = "ROL"
    description = "Rotate One Bit Left (Memory or Accumulator)"
    functionNameOverride = "RotateLeft"
    
class MOSInstr_ROR(MOSInstructionDefinition):
    name = "ROR"
    description = "Rotate One Bit Right (Memory or Accumulator)"
    functionNameOverride = "RotateRight"
    
class MOSInstr_RTI(MOSInstructionDefinition):
    name = "RTI"
    description = "Return from Interrupt"
    marksEndOfSection = True
    
class MOSInstr_RTS(MOSInstructionDefinition):
    name = "RTS"
    description = "Return from Subroutine"
    marksEndOfSection = True

class MOSInstr_SBC(MOSInstructionDefinition):
    name = "SBC"
    description = "Subtract Memory from Accumulator with Borrow"
    functionNameOverride = "SubtractFromA"
    
class MOSInstr_SEC(MOSInstructionDefinition):
    name = "SEC"
    description = "Set Carry Flag"
    functionNameOverride = "SetCarryFlag"
    
class MOSInstr_SED(MOSInstructionDefinition):
    name = "SED"
    description = "Set Decimal Mode"
    functionNameOverride = "SetDecimalFlag"
    
class MOSInstr_SEI(MOSInstructionDefinition):
    name = "SEI"
    description = "Set Interrupt Disable Status"
    functionNameOverride = "SetInterruptDisableFlag"
    
class MOSInstr_STA(MOSInstructionDefinition):
    name = "STA"
    description = "Store Accumulator in Memory"
    functionNameOverride = "StoreA"
    
class MOSInstr_STX(MOSInstructionDefinition):
    name = "STX"
    description = "Store Index X in Memory"
    functionNameOverride = "StoreX"
    
class MOSInstr_STY(MOSInstructionDefinition):
    name = "STY"
    description = "Store Index Y in Memory"
    functionNameOverride = "StoreY"
    
class MOSInstr_TAX(MOSInstructionDefinition):
    name = "TAX"
    description = "Transfer Accumulator to Index X"
    functionNameOverride = "MoveAToX"
    
class MOSInstr_TAY(MOSInstructionDefinition):
    name = "TAY"
    description = "Transfer Accumulator to Index Y"
    functionNameOverride = "MoveAToY"
    
class MOSInstr_TSX(MOSInstructionDefinition):
    name = "TSX"
    description = "Transfer Stack Pointer to Index X"
    functionNameOverride = "MoveSPToX"
    
class MOSInstr_TXA(MOSInstructionDefinition):
    name = "TXA"
    description = "Transfer Index X to Accumulator"
    functionNameOverride = "MoveXToA"
    
class MOSInstr_TXS(MOSInstructionDefinition):
    name = "TXS"
    description = "Transfer Index X to Stack Pointer"
    functionNameOverride = "MoveXToSP"
    
class MOSInstr_TYA(MOSInstructionDefinition):
    name = "TYA"
    description = "Transfer Index Y to Accumulator"
    functionNameOverride = "MoveYToA"
    
# -------------------------------------------------------------------------------------------------------------    
    
# All valid permutations of instruction definitions follow, with respective opcodes and cycle count information.
MOS_INSTRUCTION_DEFINITIONS = {}
def RegisterMOSInstrDef(instructionDefinition):
    """Registers an instruction definition in the opcode/instruction lookup table."""
    MOS_INSTRUCTION_DEFINITIONS[instructionDefinition.opcode] = instructionDefinition

def MOSInstrAll():
    """Returns a list of all instruction definitions."""
    return MOS_INSTRUCTION_DEFINITIONS.values()
    
def MOSInstrGet(opcode):
    """Returns an instruction definition given the respective opcode."""
    return MOS_INSTRUCTION_DEFINITIONS[opcode]

def MOSInstrExists(opcode):
    """Verifies that a certain opcode exists in the architecture."""
    return opcode in MOS_INSTRUCTION_DEFINITIONS

RegisterMOSInstrDef(MOSInstr_ADC(0x69, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_ADC(0x65, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_ADC(0x75, MOSAddressingMode.ZERO_PAGE_X, 4, False))
RegisterMOSInstrDef(MOSInstr_ADC(0x6D, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_ADC(0x7D, MOSAddressingMode.ABSOLUTE_X, 4, True))
RegisterMOSInstrDef(MOSInstr_ADC(0x79, MOSAddressingMode.ABSOLUTE_Y, 4, True))
RegisterMOSInstrDef(MOSInstr_ADC(0x61, MOSAddressingMode.INDIRECT_X, 6, False))
RegisterMOSInstrDef(MOSInstr_ADC(0x71, MOSAddressingMode.INDIRECT_Y, 5, True))
RegisterMOSInstrDef(MOSInstr_AND(0x29, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_AND(0x25, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_AND(0x35, MOSAddressingMode.ZERO_PAGE_X, 4, False))
RegisterMOSInstrDef(MOSInstr_AND(0x2D, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_AND(0x3D, MOSAddressingMode.ABSOLUTE_X, 4, True))
RegisterMOSInstrDef(MOSInstr_AND(0x39, MOSAddressingMode.ABSOLUTE_Y, 4, True))
RegisterMOSInstrDef(MOSInstr_AND(0x21, MOSAddressingMode.INDIRECT_X, 6, False))
RegisterMOSInstrDef(MOSInstr_AND(0x31, MOSAddressingMode.INDIRECT_Y, 5, False))
RegisterMOSInstrDef(MOSInstr_ASL(0x0A, MOSAddressingMode.ACCUMULATOR, 2, False))
RegisterMOSInstrDef(MOSInstr_ASL(0x06, MOSAddressingMode.ZERO_PAGE, 5, False))
RegisterMOSInstrDef(MOSInstr_ASL(0x16, MOSAddressingMode.ZERO_PAGE_X, 6, False))
RegisterMOSInstrDef(MOSInstr_ASL(0x0E, MOSAddressingMode.ABSOLUTE, 6, False))
RegisterMOSInstrDef(MOSInstr_ASL(0x1E, MOSAddressingMode.ABSOLUTE_X, 7, False))
RegisterMOSInstrDef(MOSInstr_BCC(0x90, MOSAddressingMode.RELATIVE, 2, True))
RegisterMOSInstrDef(MOSInstr_BCS(0xB0, MOSAddressingMode.RELATIVE, 2, True))
RegisterMOSInstrDef(MOSInstr_BEQ(0xF0, MOSAddressingMode.RELATIVE, 2, True))
RegisterMOSInstrDef(MOSInstr_BIT(0x24, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_BIT(0x2C, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_BMI(0x30, MOSAddressingMode.RELATIVE, 2, True))
RegisterMOSInstrDef(MOSInstr_BNE(0xD0, MOSAddressingMode.RELATIVE, 2, True))
RegisterMOSInstrDef(MOSInstr_BPL(0x10, MOSAddressingMode.RELATIVE, 2, True))
RegisterMOSInstrDef(MOSInstr_BRK(0x00, MOSAddressingMode.IMPLIED, 7, False))
RegisterMOSInstrDef(MOSInstr_BVC(0x50, MOSAddressingMode.RELATIVE, 2, True))
RegisterMOSInstrDef(MOSInstr_BVS(0x70, MOSAddressingMode.RELATIVE, 2, True))
RegisterMOSInstrDef(MOSInstr_CLC(0x18, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_CLD(0xD8, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_CLI(0x58, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_CLV(0xB8, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_CMP(0xC9, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_CMP(0xC5, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_CMP(0xD5, MOSAddressingMode.ZERO_PAGE_X, 4, False))
RegisterMOSInstrDef(MOSInstr_CMP(0xCD, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_CMP(0xDD, MOSAddressingMode.ABSOLUTE_X, 4, True))
RegisterMOSInstrDef(MOSInstr_CMP(0xD9, MOSAddressingMode.ABSOLUTE_Y, 4, True))
RegisterMOSInstrDef(MOSInstr_CMP(0xC1, MOSAddressingMode.INDIRECT_X, 6, False))
RegisterMOSInstrDef(MOSInstr_CMP(0xD1, MOSAddressingMode.INDIRECT_Y, 5, True))
RegisterMOSInstrDef(MOSInstr_CPX(0xE0, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_CPX(0xE4, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_CPX(0xEC, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_CPY(0xC0, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_CPY(0xC4, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_CPY(0xCC, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_DEC(0xC6, MOSAddressingMode.ZERO_PAGE, 5, False))
RegisterMOSInstrDef(MOSInstr_DEC(0xD6, MOSAddressingMode.ZERO_PAGE_X, 6, False))
RegisterMOSInstrDef(MOSInstr_DEC(0xCE, MOSAddressingMode.ABSOLUTE, 6, False))
RegisterMOSInstrDef(MOSInstr_DEC(0xDE, MOSAddressingMode.ABSOLUTE_X, 7, False))
RegisterMOSInstrDef(MOSInstr_DEX(0xCA, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_DEY(0x88, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_EOR(0x49, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_EOR(0x45, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_EOR(0x55, MOSAddressingMode.ZERO_PAGE_X, 4, False))
RegisterMOSInstrDef(MOSInstr_EOR(0x4D, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_EOR(0x5D, MOSAddressingMode.ABSOLUTE_X, 4, True))
RegisterMOSInstrDef(MOSInstr_EOR(0x59, MOSAddressingMode.ABSOLUTE_Y, 4, True))
RegisterMOSInstrDef(MOSInstr_EOR(0x41, MOSAddressingMode.INDIRECT_X, 6, False))
RegisterMOSInstrDef(MOSInstr_EOR(0x51, MOSAddressingMode.INDIRECT_Y, 5, True))
RegisterMOSInstrDef(MOSInstr_INC(0xE6, MOSAddressingMode.ZERO_PAGE, 5, False))
RegisterMOSInstrDef(MOSInstr_INC(0xF6, MOSAddressingMode.ZERO_PAGE_X, 6, False))
RegisterMOSInstrDef(MOSInstr_INC(0xEE, MOSAddressingMode.ABSOLUTE, 6, False))
RegisterMOSInstrDef(MOSInstr_INC(0xFE, MOSAddressingMode.ABSOLUTE_X, 7, False))
RegisterMOSInstrDef(MOSInstr_INX(0xE8, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_INY(0xC8, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_JMP(0x4C, MOSAddressingMode.ABSOLUTE, 3, False))
RegisterMOSInstrDef(MOSInstr_JMP(0x6C, MOSAddressingMode.INDIRECT, 4, False))
RegisterMOSInstrDef(MOSInstr_JSR(0x20, MOSAddressingMode.ABSOLUTE, 6, False))
RegisterMOSInstrDef(MOSInstr_LDA(0xA9, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_LDA(0xA5, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_LDA(0xB5, MOSAddressingMode.ZERO_PAGE_X, 4, False))
RegisterMOSInstrDef(MOSInstr_LDA(0xAD, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_LDA(0xBD, MOSAddressingMode.ABSOLUTE_X, 4, True))
RegisterMOSInstrDef(MOSInstr_LDA(0xB9, MOSAddressingMode.ABSOLUTE_Y, 4, True))
RegisterMOSInstrDef(MOSInstr_LDA(0xA1, MOSAddressingMode.INDIRECT_X, 6, False))
RegisterMOSInstrDef(MOSInstr_LDA(0xB1, MOSAddressingMode.INDIRECT_Y, 5, True))
RegisterMOSInstrDef(MOSInstr_LDX(0xA2, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_LDX(0xA6, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_LDX(0xB6, MOSAddressingMode.ZERO_PAGE_Y, 4, False))
RegisterMOSInstrDef(MOSInstr_LDX(0xAE, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_LDX(0xBE, MOSAddressingMode.ABSOLUTE_Y, 4, True))
RegisterMOSInstrDef(MOSInstr_LDY(0xA0, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_LDY(0xA4, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_LDY(0xB4, MOSAddressingMode.ZERO_PAGE_X, 4, False))
RegisterMOSInstrDef(MOSInstr_LDY(0xAC, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_LDY(0xBC, MOSAddressingMode.ABSOLUTE_X, 4, True))
RegisterMOSInstrDef(MOSInstr_LSR(0x4A, MOSAddressingMode.ACCUMULATOR, 2, False))
RegisterMOSInstrDef(MOSInstr_LSR(0x46, MOSAddressingMode.ZERO_PAGE, 5, False))
RegisterMOSInstrDef(MOSInstr_LSR(0x56, MOSAddressingMode.ZERO_PAGE_X, 6, False))
RegisterMOSInstrDef(MOSInstr_LSR(0x4E, MOSAddressingMode.ABSOLUTE, 6, False))
RegisterMOSInstrDef(MOSInstr_LSR(0x5E, MOSAddressingMode.ABSOLUTE_X, 7, False))
RegisterMOSInstrDef(MOSInstr_NOP(0xEA, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_ORA(0x09, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_ORA(0x05, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_ORA(0x15, MOSAddressingMode.ZERO_PAGE_X, 4, False))
RegisterMOSInstrDef(MOSInstr_ORA(0x0D, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_ORA(0x1D, MOSAddressingMode.ABSOLUTE_X, 4, True))
RegisterMOSInstrDef(MOSInstr_ORA(0x19, MOSAddressingMode.ABSOLUTE_Y, 4, True))
RegisterMOSInstrDef(MOSInstr_ORA(0x01, MOSAddressingMode.INDIRECT_X, 6, False))
RegisterMOSInstrDef(MOSInstr_ORA(0x11, MOSAddressingMode.INDIRECT_Y, 5, True))
RegisterMOSInstrDef(MOSInstr_PHA(0x48, MOSAddressingMode.IMPLIED, 3, False))
RegisterMOSInstrDef(MOSInstr_PHP(0x08, MOSAddressingMode.IMPLIED, 3, False))
RegisterMOSInstrDef(MOSInstr_PLA(0x68, MOSAddressingMode.IMPLIED, 4, False))
RegisterMOSInstrDef(MOSInstr_PLP(0x28, MOSAddressingMode.IMPLIED, 4, False))
RegisterMOSInstrDef(MOSInstr_ROL(0x2A, MOSAddressingMode.ACCUMULATOR, 2, False))
RegisterMOSInstrDef(MOSInstr_ROL(0x26, MOSAddressingMode.ZERO_PAGE, 5, False))
RegisterMOSInstrDef(MOSInstr_ROL(0x36, MOSAddressingMode.ZERO_PAGE_X, 6, False))
RegisterMOSInstrDef(MOSInstr_ROL(0x2E, MOSAddressingMode.ABSOLUTE, 6, False))
RegisterMOSInstrDef(MOSInstr_ROL(0x3E, MOSAddressingMode.ABSOLUTE_X, 7, False))
RegisterMOSInstrDef(MOSInstr_ROR(0x6A, MOSAddressingMode.ACCUMULATOR, 2, False))
RegisterMOSInstrDef(MOSInstr_ROR(0x66, MOSAddressingMode.ZERO_PAGE, 5, False))
RegisterMOSInstrDef(MOSInstr_ROR(0x76, MOSAddressingMode.ZERO_PAGE_X, 6, False))
RegisterMOSInstrDef(MOSInstr_ROR(0x6E, MOSAddressingMode.ABSOLUTE, 6, False))
RegisterMOSInstrDef(MOSInstr_ROR(0x7E, MOSAddressingMode.ABSOLUTE_X, 7, False))
RegisterMOSInstrDef(MOSInstr_RTI(0x40, MOSAddressingMode.IMPLIED, 6, False))
RegisterMOSInstrDef(MOSInstr_RTS(0x60, MOSAddressingMode.IMPLIED, 6, False))
RegisterMOSInstrDef(MOSInstr_SBC(0xE9, MOSAddressingMode.IMMEDIATE, 2, False))
RegisterMOSInstrDef(MOSInstr_SBC(0xE5, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_SBC(0xF5, MOSAddressingMode.ZERO_PAGE_X, 4, False))
RegisterMOSInstrDef(MOSInstr_SBC(0xED, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_SBC(0xFD, MOSAddressingMode.ABSOLUTE_X, 4, True))
RegisterMOSInstrDef(MOSInstr_SBC(0xF9, MOSAddressingMode.ABSOLUTE_Y, 4, True))
RegisterMOSInstrDef(MOSInstr_SBC(0xE1, MOSAddressingMode.INDIRECT_X, 6, False))
RegisterMOSInstrDef(MOSInstr_SBC(0xF1, MOSAddressingMode.INDIRECT_Y, 5, True))
RegisterMOSInstrDef(MOSInstr_SEC(0x38, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_SED(0xF8, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_SEI(0x78, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_STA(0x85, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_STA(0x95, MOSAddressingMode.ZERO_PAGE_X, 4, False))
RegisterMOSInstrDef(MOSInstr_STA(0x8D, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_STA(0x9D, MOSAddressingMode.ABSOLUTE_X, 5, False))
RegisterMOSInstrDef(MOSInstr_STA(0x99, MOSAddressingMode.ABSOLUTE_Y, 5, False))
RegisterMOSInstrDef(MOSInstr_STA(0x81, MOSAddressingMode.INDIRECT_X, 6, False))
RegisterMOSInstrDef(MOSInstr_STA(0x91, MOSAddressingMode.INDIRECT_Y, 6, False))
RegisterMOSInstrDef(MOSInstr_STX(0x86, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_STX(0x96, MOSAddressingMode.ZERO_PAGE_Y, 4, False))
RegisterMOSInstrDef(MOSInstr_STX(0x8E, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_STY(0x84, MOSAddressingMode.ZERO_PAGE, 3, False))
RegisterMOSInstrDef(MOSInstr_STY(0x94, MOSAddressingMode.ZERO_PAGE_X, 4, False))
RegisterMOSInstrDef(MOSInstr_STY(0x8C, MOSAddressingMode.ABSOLUTE, 4, False))
RegisterMOSInstrDef(MOSInstr_TAX(0xAA, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_TAY(0xA8, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_TSX(0xBA, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_TXA(0x8A, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_TXS(0x9A, MOSAddressingMode.IMPLIED, 2, False))
RegisterMOSInstrDef(MOSInstr_TYA(0x98, MOSAddressingMode.IMPLIED, 2, False))
