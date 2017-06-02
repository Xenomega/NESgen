from MOS6502Instructions import *
from NESMemory import NESMemory
from PRGROM import *
from dis import Instruction
class iNESROMDisassembler:
    ALLOW_FUNCTION_NAME_OVERRIDES = True
    ALLOW_KNOWN_MEMORY_ACCESS_LABELS = True
    ALLOW_RUNTIME_LOCATIONS = True
    OUTPUT_FULL_PRGROM_DATA = False
    class IOOperationType(Enum):
        """Describes whether an IO operation is a read or write."""
        READ = 0
        WRITE = 1
        
    def __GetCodeSectionLabels(self, rom, prgRom, address):
        """Returns a list of labels for this address."""
        # NOTE: Multiple labels are important since multiple interrupts can share the same code but we require individual labels for them to handle interrupts properly.
        labels = []
        offset = NESMemory.pointerToOffset(rom, address)
        isStartingLocation = address in prgRom.codeSections
        
        # Check and add interrupt labels.
        if(offset == NESMemory.pointerToOffset(rom, prgRom.interruptNMI)):
            labels.append("FUNCTION_NMI")
        if(offset == NESMemory.pointerToOffset(rom, prgRom.interruptReset)):
            labels.append("FUNCTION_RESET")
        if(offset == NESMemory.pointerToOffset(rom, prgRom.interruptIRQ)):
            labels.append("FUNCTION_IRQ")
            
        # If we don't have any labels yet, set them accordingly.
        if(len(labels) == 0):
            if(isStartingLocation):
                if(prgRom.codeSections[address].referenceType == PRGROMCodeSectionType.FUNCTION):
                    labels.append("FUNCTION_" + hex(address)[2:])
                else:
                    labels.append("LOCATION_" + hex(address)[2:])
            elif(self.ALLOW_RUNTIME_LOCATIONS):
                labels.append("____runtimeloc_" + hex(address)[2:])
        return labels
    
    def __GetInstructionFunction(self, instructionOrDefinition):
        """Obtains an instruction function for it's environment given an instruction or instruction definition."""
        instrDefinition = instructionOrDefinition
        if(type(instrDefinition) is MOSInstruction):
            instrDefinition = instrDefinition.definition
        return "MOSInstr_{}".format(instrDefinition.name)
    
    def __GetDataSectionLabel(self, address):
        """Returns a label (value name) for data section at the given address."""
        return "DATA_" + hex(address)[2:]
    
    def __GetCodeSectionLabelID(self, label):
        """Obtains a macro given a code section label name."""
        return "ID_" + label

    def __GetAddressMacroLabel(self, addr, operationType):
        """Obtains an label for a certain address in memory."""
        if(self.ALLOW_KNOWN_MEMORY_ACCESS_LABELS and operationType == self.IOOperationType.READ):
            return {
                        0x2002 : "PPUSTATUS_REGISTER",
                        0x2004 : "PPUOAMDATA_REGISTER",
                        0x2007 : "PPUDATA_REGISTER",
                        0x4015 : "APU_STATUS_REGISTER",
                        0x4016 : "CONTROLLER1_STATE_REGISTER",
                        0x4017 : "CONTROLLER2_STATE_REGISTER",
                        
                }.get(addr, hex(addr))
        elif(self.ALLOW_KNOWN_MEMORY_ACCESS_LABELS and operationType == self.IOOperationType.WRITE):
            return {
                        0x2000 : "PPUCTRL_REGISTER",
                        0x2001 : "PPUMASK_REGISTER",
                        0x2003 : "PPUOAMADDR_REGISTER",
                        0x2004 : "PPUOAMDATA_REGISTER",
                        0x2005 : "PPUSCROLL_REGISTER",
                        0x2006 : "PPUADDR_REGISTER",
                        0x2007 : "PPUDATA_REGISTER",
                        0x4000 : "APU_PULSE1_CHANNEL_REGISTER0",
                        0x4001 : "APU_PULSE1_CHANNEL_REGISTER1",
                        0x4002 : "APU_PULSE1_CHANNEL_REGISTER2",
                        0x4003 : "APU_PULSE1_CHANNEL_REGISTER3",
                        0x4004 : "APU_PULSE2_CHANNEL_REGISTER0",
                        0x4005 : "APU_PULSE2_CHANNEL_REGISTER1",
                        0x4006 : "APU_PULSE2_CHANNEL_REGISTER2",
                        0x4007 : "APU_PULSE2_CHANNEL_REGISTER3",
                        0x4008 : "APU_TRIANGLE_CHANNEL_REGISTER0",
                        0x400A : "APU_TRIANGLE_CHANNEL_REGISTER1",
                        0x400B : "APU_TRIANGLE_CHANNEL_REGISTER2",
                        0x400C : "APU_NOISE_CHANNEL_REGISTER0",
                        0x400E : "APU_NOISE_CHANNEL_REGISTER1",
                        0x400F : "APU_NOISE_CHANNEL_REGISTER2",
                        0x4010 : "APU_DMC_CHANNEL_REGISTER0",
                        0x4011 : "APU_DMC_CHANNEL_REGISTER1",
                        0x4012 : "APU_DMC_CHANNEL_REGISTER2",
                        0x4013 : "APU_DMC_CHANNEL_REGISTER3",
                        0x4014 : "PPUOAMDMA_REGISTER",
                        0x4015 : "APU_CONTROL_REGISTER",
                        0x4016 : "CONTROLLER_STROBE_REGISTER",
                        0x4017 : "APU_FRAME_COUNTER_REGISTER",
                        
                }.get(addr, hex(addr))
        return hex(addr)
    
    def DisassembleToASM(self, rom):
        """Disassembles the given ROM to an .ASM-like format."""
        # Parse the PRG-ROM into it's respective code/data sections.
        prgRom = PRGROM(rom)
    
        # Print the base address portion of the .ASM file
        print(".org " + hex(NESMemory.PRG_ROM_START_ADDR))

        # Loop from start to finish of PRG-ROM
        address = NESMemory.offsetToPointer(0)
        end = address + prgRom.size
        while address < end:
            # Determine if the section is code or data and export accordingly.
            if(address in prgRom.codeSections):
                # It's a code section, print out any labels for this address accordingly.
                labels = self.__GetCodeSectionLabels(rom, prgRom, address)
                for label in labels:
                    print(label + ":")
                    
                # Obtain the code section
                codeSection = prgRom.codeSections[address]
                for instruction in codeSection.instructions:
                    # If it's a jump or branch, we want specific output to a label at that address.
                    if(instruction.definition.isJumpOrBranch):
                        jumpLabelAddress = NESMemory.offsetToPointer(prgRom.resolveJumpOffset(rom, instruction))
                        print("\t" + instruction.definition.name + " " + self.__GetCodeSectionLabels(rom, prgRom, jumpLabelAddress)[0])
                    else:
                        # Otherwise we just output the instruction's text (normal asm command with byte literals)
                        print("\t" + str(instruction))
                address += codeSection.getSize()
            else:
                dataSection = prgRom.dataSections[address]
                print("\t.db ", end="")
                for x in range(0, dataSection.getSize()):
                    print("0x{:02x}".format(dataSection.data[x]), end=", " if x < (dataSection.getSize() - 1) else "")
                print("")
                address += dataSection.getSize()
                
    def DisassembleToC(self, rom, sourcePath, headerPath):
        """Disassembles the given ROM to C files for use with NESsys."""
        # Parse the PRG-ROM into it's respective code/data sections.
        prgRom = PRGROM(rom)
        # And generate the source and header file
        header = self.__GenerateCHeader(rom, prgRom)
        source = self.__GenerateCSource(rom, prgRom)
        
        # And save them accordingly.
        fSource = open(sourcePath, "w")
        fSource.write(source)
        fSource.close()
        
        fHeader = open(headerPath, "w")
        fHeader.write(header)
        fHeader.close()
        
    def __GenerateCByteArray(self, data):
        byteStr = ""
        x = 0
        while (x < len(data)):
            if(x % 0x10 == 0):
                byteStr += "\n\t"
                    
            byteStr += "0x{:02x}".format(data[x])
            if(x != len(data) - 1):
                byteStr += ", "
            x += 1
        return byteStr
    
    def __GenerateCHeader(self, rom, prgRom):
        """Creates a header for a NES ROM to disassemble."""
        # Generate header
        header = """
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
"""
        for address in prgRom.codeSections.keys():
            labels = self.__GetCodeSectionLabels(rom, prgRom, address)
            for label in labels:
                header += "#define {0:30}{1}\n".format(self.__GetCodeSectionLabelID(label), hex(address))
        header +=  """
        
// ---------------------------------
// Function Mappings
// ---------------------------------
"""
        # Obtain an instruction definition of every type and output it's overriden name we'll macro (if none)
        if(self.ALLOW_FUNCTION_NAME_OVERRIDES):
            instructionTypes = set([])
            instructionDefinitions = MOSInstrAll()
            for instrDef in instructionDefinitions:
                if(type(instrDef) in instructionTypes):
                    continue
                instructionTypes.add(type(instrDef))
                if(instrDef.functionNameOverride != None):
                    header += "#define {0:30}{1}\n".format(instrDef.functionNameOverride, self.__GetInstructionFunction(instrDef))
        header += """
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
"""
        return header
    
    def __GenerateCInstructionCode(self, rom, prgRom, instruction):
        """Generates C code for a given instruction."""
        code = ""
        instrType = type(instruction.definition)
        syncStr = "sync({});".format(instruction.definition.cycles) # cpu_sync calls our interrupts, rendering, etc.
        # Special cases: { MOSInstr_BCC, MOSInstr_BCS, MOSInstr_BEQ, MOSInstr_BMI, MOSInstr_BNE, MOSInstr_BPL, MOSInstr_BVC, MOSInstr_BVS, MOSInstr_JMP, MOSInstr_JSR, MOSInstr_RTI, MOSInstr_RTS }
        # Also need to check cases where
        if(instrType is MOSInstr_RTI):
            code = "{} return TRUE;".format(syncStr)
            syncStr = ""
        elif(instrType is MOSInstr_RTS):
            code = "{} return FALSE;".format(syncStr)
            syncStr = ""
        elif(instruction.definition.isJumpOrBranch):
            # If it's not an indirect jump, it will be relative or absolute so we know where we'll jump to and can use an appropriate label.
            if(instruction.definition.mode != MOSAddressingMode.INDIRECT):
                pointer = NESMemory.offsetToPointer(prgRom.resolveJumpOffset(rom, instruction))
                label = self.__GetCodeSectionLabels(rom, prgRom, pointer)[0]
                if(instrType is MOSInstr_JMP):
                    code = "{}\n\tgoto {};".format(syncStr, label) # TODO: INDIRECT JMP
                    syncStr = ""
                elif(instrType is MOSInstr_JSR):
                    macroID = self.__GetCodeSectionLabelID(label)
                    code = "{} if(game_execute({})) return TRUE;".format(syncStr, macroID)
                    syncStr = ""
                else:
                    # It must be a conditional branch, figure out our condition
                    condition = {
                        MOSInstr_BCC : "!cpu_get_flag(CPU_FLAG_CARRY)",
                        MOSInstr_BCS : "cpu_get_flag(CPU_FLAG_CARRY)",
                        MOSInstr_BEQ : "cpu_get_flag(CPU_FLAG_ZERO)",
                        MOSInstr_BMI : "cpu_get_flag(CPU_FLAG_SIGN)",
                        MOSInstr_BNE : "!cpu_get_flag(CPU_FLAG_ZERO)",
                        MOSInstr_BPL : "!cpu_get_flag(CPU_FLAG_SIGN)",
                        MOSInstr_BVC : "!cpu_get_flag(CPU_FLAG_OVERFLOW)",
                        MOSInstr_BVS : "cpu_get_flag(CPU_FLAG_OVERFLOW)",
                    }[instrType]
                    # If we branch we add an additional cycle, otherwise we don't.
                    code = "if({}) {{ {} goto {}; }}".format(condition, "sync({});".format(instruction.definition.cycles + 1), label)
            else:
                # The only indirect jump is the JMP instruction. This will not have a label, and requires special code.
                code = "{}\n\tjumpAddress = cpu_read16({}); goto Jump;".format(syncStr, hex(instruction.operand))
                syncStr = ""
        else:
            # Obtain our instruction function name.
            if(self.ALLOW_FUNCTION_NAME_OVERRIDES and instruction.definition.functionNameOverride != None):
                instrFuncName = instruction.definition.functionNameOverride
            else:
                instrFuncName = self.__GetInstructionFunction(instruction.definition)
            code = "{}({{}})".format(instrFuncName)
            mode = instruction.definition.mode
            storesBack = instrType in {MOSInstr_ASL, MOSInstr_DEC, MOSInstr_INC, MOSInstr_LSR, MOSInstr_ROL, MOSInstr_ROR}
            
            isStoreInstruction = instrType in {MOSInstr_STA, MOSInstr_STX, MOSInstr_STY}
            usesValue = not isStoreInstruction
            argument = ""
            if(mode == MOSAddressingMode.ABSOLUTE or mode == MOSAddressingMode.ZERO_PAGE):
                if(isStoreInstruction):
                    argument = self.__GetAddressMacroLabel(instruction.operand, self.IOOperationType.WRITE)
                else:
                    argument = self.__GetAddressMacroLabel(instruction.operand, self.IOOperationType.READ)
                if(usesValue): argument = "cpu_read8({})".format(argument)
                code = code.format(argument)
                if(storesBack): code = "cpu_write8({}, {})".format(self.__GetAddressMacroLabel(instruction.operand, self.IOOperationType.WRITE), code)
            elif(mode == MOSAddressingMode.ZERO_PAGE_X):
                argument = "(registers.X + {}) & 0xFF".format(hex(instruction.operand))
                if(usesValue): argument = "cpu_read8({})".format(argument)
                code = code.format(argument) # TODO: Check page crossing boundary
                if(storesBack): code = "cpu_write8((registers.X + {}) & 0xFF, {})".format(hex(instruction.operand), code)
            elif(mode == MOSAddressingMode.ABSOLUTE_X):
                argument = "registers.X + {}".format(hex(instruction.operand))
                if(usesValue): argument = "cpu_read8({})".format(argument)
                code = code.format(argument) # TODO: Check page crossing boundary
                if(storesBack): code = "cpu_write8(registers.X + {}, {})".format(hex(instruction.operand), code)
            elif(mode == MOSAddressingMode.ZERO_PAGE_Y):
                argument = "(registers.Y + {}) & 0xFF".format(hex(instruction.operand))
                if(usesValue): argument = "cpu_read8({})".format(argument)
                code = code.format(argument) # TODO: Check page crossing boundary
                if(storesBack): code = "cpu_write8((registers.Y + {}) & 0xFF, {})".format(hex(instruction.operand), code)
            elif(mode == MOSAddressingMode.ABSOLUTE_Y):
                argument = "registers.Y + {}".format(hex(instruction.operand))
                if(usesValue): argument = "cpu_read8({})".format(argument)
                code = code.format(argument) # TODO: Check page crossing boundary
                if(storesBack): code = "cpu_write8(registers.Y + {}, {})".format(hex(instruction.operand), code)
            elif(mode == MOSAddressingMode.ACCUMULATOR):
                argument = "registers.A"
                code = code.format(argument)
                if(storesBack): code = "registers.A = {}".format(code)
            elif(mode == MOSAddressingMode.IMMEDIATE):
                argument = hex(instruction.operand)
                code = code.format(argument)
                if(storesBack): raise ValueError("Cannot store back to an immediate value.")
            elif(mode == MOSAddressingMode.INDIRECT_X):
                argument = "cpu_read16((registers.X + {}) & 0xFF)".format(hex(instruction.operand))
                if(usesValue): argument = "cpu_read8({})".format(argument)
                code = code.format(argument) # TODO: Check page crossing boundary
                if(storesBack): code = "cpu_write8(cpu_read16((registers.X + {}) & 0xFF), {})".format(hex(instruction.operand), code)
            elif(mode == MOSAddressingMode.INDIRECT_Y):
                argument = "registers.Y + cpu_read16({})".format(hex(instruction.operand))
                if(usesValue): argument = "cpu_read8({})".format(argument)
                code = code.format(argument)
                if(storesBack): code = "cpu_write8(registers.Y + cpu_read16({}), {})".format(hex(instruction.operand), code)
            else:
                code = code.format(argument)

            # IMPLIED has no operands, RELATIVE + INDIRECT are only used by jumps which are specially handled elsewhere.
            code += ";"
        if(code != ""):
            code = "\t{} // {}\n".format(code, instruction.definition.description);
            if(syncStr != ""):
                code += "\t" + syncStr + "\n"
                
        return code
    def __GenerateCSource(self, rom, prgRom):
        """Disassembles the given ROM to C Source for use with NESsys."""
        source = """
#include <stdio.h>
#include "game.h"

#if !APPLICATION_USE_TEMPLATE
// ---------------------------------
// Functions
// ---------------------------------
BOOL game_execute(USHORT jumpAddress)
{ 
    // Go to our jump table first to find out where to execute.
    // We do this to display game code first and hide the bloated jump table for later.
    goto Jump;
    
    // Game code follows...
"""
        # Generate our program code (from lower to higher addresses)
        for sectionAddress in sorted(prgRom.codeSections.keys()):
            # Now for each instruction...
            for instruction in prgRom.codeSections[sectionAddress].instructions:
                # Output all goto labels first for this address
                labels = self.__GetCodeSectionLabels(rom, prgRom, instruction.address)
                for label in labels:
                    source += "{}:\n".format(label)
                # And output out the instruction code.
                source += self.__GenerateCInstructionCode(rom, prgRom, instruction)
        # Close up our function
        source += """
        
    // Switch table for functions here first.
    // This jumps to the appropriate code location based off a given address.
    // Absolute jumps jump directly, runtime calculated (indirect) jumps use this table.
    // Anything that calls this function obviously also uses this to begin in the correct location since goto's are local.
    // NOTE: We can only put one case per address even though there may be multiple labels at a given address.
    Jump:
    switch(jumpAddress)
    {
"""
        # Print our goto switch table..
        
        # First we print common (known) jumps just in case our compiler puts these sequentially.
        for address in prgRom.codeSections:
                # Print our goto case
                label = self.__GetCodeSectionLabels(rom, prgRom, address)[0]
                source += "\tcase {}:\n".format(self.__GetCodeSectionLabelID(label))
                # If our ROM is only one bank in size, it's mirrored onto the second bank as well, so we maintain support for this.
                if(NESMemory.isMirroredROM(rom)):
                    source += "\tcase {}:\n".format(hex((address - NESMemory.PRG_ROM_FIRST_BANK_ADDR) + NESMemory.PRG_ROM_SECOND_BANK_ADDR))
                source += "\t\tgoto {};\n".format(label)
                
        # Second we print all runtime locations just in case our game calculates a jump offset.
        if(self.ALLOW_RUNTIME_LOCATIONS):
            for address in prgRom.codeSections:
                # Now for each location in our section (each instruction)
                for instruction in prgRom.codeSections[address].instructions:
                    # Verify we haven't already printed this (first instruction of a code section)
                    if address == instruction.address:
                        continue
                    # Print our goto case
                    label = self.__GetCodeSectionLabels(rom, prgRom, instruction.address)[0]
                    source += "\tcase {}:\n".format(hex(instruction.address))
                    # If our ROM is only one bank in size, it's mirrored onto the second bank as well, so we maintain support for this.
                    if(NESMemory.isMirroredROM(rom)):
                        source += "\tcase {}:\n".format(hex((instruction.address - NESMemory.PRG_ROM_FIRST_BANK_ADDR) + NESMemory.PRG_ROM_SECOND_BANK_ADDR))
                    source += "\t\tgoto {};\n".format(label)
        # Finally, print our default case (error)
        source += "\tdefault:\n"
        source += "\t\terror(\"{}\", jumpAddress);\n".format("Attempted to jump to an non-executable location 0x%04x. This location may have been calculated at runtime and not supported by the compiler, the ROM may be faulty, or improper emulation of some component has caused undesirable runtime effects.")
        source += """
    }


    return FALSE;
}

// ---------------------------------
// Data
// ---------------------------------
/*
 * Describes the layout of the nametables and which should be mirrored.
 */
enum MIRRORINGTYPE mirroringType = """ 
        # Add our PPU mirroring type.
        source += "{};".format(rom.mirroring.name)
        source += """
/*
 * Translation Lookaside Buffer
 * Used for address lookups to map virtual PRG-ROM address spaces to system memory.
 */
struct TLBEntry gameTLB[] =
{
"""
        # Define our sections our data will be present relative to PRG-ROM start (0x8000)
        prgRomLocations = [0] 
        
        # If we are to mirror the 8kb rom into both 16kb spaces, we also want to mirror it another 8kb further.
        if NESMemory.isMirroredROM(rom):
            prgRomLocations.append(rom.PRG_ROM_BANK_SIZE)
        
        # Determine if we're outputting all PRG-ROM data or just determined data sections.
        if(self.OUTPUT_FULL_PRGROM_DATA):
            for prgRomLocation in prgRomLocations:
                source += "\t{{  {}, {}, {} }},\n".format(hex(NESMemory.PRG_ROM_START_ADDR + prgRomLocation), hex(NESMemory.PRG_ROM_START_ADDR + prgRomLocation + len(rom.prgRom)), "prgRomData")
        else:
            # Loop for each data section and output it
            for prgRomLocation in prgRomLocations:
                prgRomDataOffset = 0
                for address, dataSection in prgRom.dataSections.items():
                    startAddr = prgRomLocation + address
                    endAddr = startAddr + dataSection.getSize()
                    resolvedLocation = "(prgRomData + {})".format(hex(prgRomDataOffset))
                    prgRomDataOffset += dataSection.getSize()
                    source += "\t{{ {}, {}, {} }},\n".format(hex(startAddr), hex(endAddr), resolvedLocation)
        source += """
};
UINT gameTLBSize = sizeof(gameTLB) / sizeof(struct TLBEntry);

"""

        # PRG-ROM (Data):
        # We're aiming to output all data sections as a singular data array since data sections are not analyzed further
        # and are likely even further segmented than our current scheme (segmented by code sections). 
        # Since no further analysis is done, we store them in one location to keep it clean and map to them.
        source += "BYTE prgRomData[] = {"
        
        # Determine if we're outputting all PRG-ROM data or just determined data sections.
        if(self.OUTPUT_FULL_PRGROM_DATA):
            source += self.__GenerateCByteArray(rom.prgRom)
        else:
            prgRomData = []
            for address, dataSection in prgRom.dataSections.items():
                prgRomData.extend(dataSection.data)
            source += self.__GenerateCByteArray(prgRomData)
            
        source += "\n};\n\n"
        source += """UINT prgRomDataSize = sizeof(prgRomData);
"""
        
        # CHR-ROM:
        source += "BYTE chrRom[] = {"
        source += self.__GenerateCByteArray(rom.chrRom)
        source += "\n};\n"
        source += """UINT chrRomSize = sizeof(chrRom);

#endif"""
        return source