# NES PRG-ROM support (Program ROM, aka Executable/Code/Data Section)
# Reference: https://wiki.nesdev.com/w/index.php/INES
from MOS6502Instructions import *
from NESMemory import NESMemory

class PRGROMCodeSectionType(Enum):
    """Describes the type of code section (based off how it is called). Higher indexes override lower if multiple."""
    LOCATION = 0
    FUNCTION = 1
    INTERRUPT = 2

class PRGROMCodeSection:
    """Describes a code section in the PRG-ROM. Delimited by certain control-flow based instructions or jump locations."""
    referenceType = None
    address = 0 # address in memory
    offset = 0 # offset in PRG-ROM
    referencedBy = None # addresses
    instructions = None # instructions in sequential order
    done = False
    
    def __init__(self, offset):
        """Initializes the code section to begin building."""
        self.offset = offset
        self.address = NESMemory.offsetToPointer(self.offset)
        self.referencedBy = set([]) # addresses
        self.instructions = [] # instructions in sequential order
        self.done = False
    
    def addInstruction(self, instruction):
        """Appends the instruction."""
        self.instructions.append(instruction)
        
    def addRef(self, referenceType, referencedByOffset):
        """Adds a reference to the code section from the given offset, and changes code section type accordingly."""
        self.referencedBy.add(referencedByOffset)
        self.sectionType = max(self.referenceType, referenceType) if self.referenceType != None else referenceType
        
    def getSize(self):
        """Obtains the size of the code section."""
        if(len(self.instructions) > 0):
            # The last instruction (in a sequential list) will be the end. Set size to point after this instruction.
            return (self.instructions[-1].offset + self.instructions[-1].definition.size) - self.offset
        else:
            return 0

    def split(self, offset):
        """
        Splits the code section at the instruction at the given offset. 
        Returns None if the offset is not in the code section or isn't aligned to an instruction.
        """
        # Look for an instruction at the given offset
        for x in range(0, len(self.instructions)):
            if(self.instructions[x].offset == offset):
                newSection = PRGROMCodeSection(offset)
                newSection.instructions = self.instructions[x:]
                self.instructions = self.instructions[:x]
                newSection.done = self.done
                self.done = True
                # If we split a JMP (infinite loop on itself), we want to detect our new split part is done too
                if(not newSection.done and len(newSection.instructions) > 0):
                    newSection.done = newSection.instructions[-1].definition.marksEndOfSection
                return newSection
            
        # We couldn't find an instruction aligned at the offset so return nothing.
        # This indicates we may have overlapping instructions which we can handle separately.
        return None
    
class PRGROMDataSection:
    """Describes a data section in the PRG-ROM. Determined by sections which are not code sections."""
    address = 0 # address in memory
    offset = 0 # offset in PRG-ROM
    data = None # byte array
    
    def __init__(self, offset, data):
        """Initializes the code section to begin building."""
        self.offset = offset
        self.address = NESMemory.offsetToPointer(offset)
        self.data = data
        
    def getSize(self):
        """Obtains the size of the code section."""
        return len(self.data)
    
class PRGROM:
    """Describes a PRG-ROM within an iNES ROM. Parses underlying code/data sections."""
    codeSections = {} # address : PRGROMCodeSection
    dataSections = {} # address : PRGROMDataSection
    interruptNMI, interruptReset, interruptIRQ = None, None, None
    size = 0
    def __init__(self, rom):
        """Initializes the appropriate members to begin parsing underlying structure."""
        # Determine code sections
        self.size = len(rom.prgRom)
        self.codeSections = {}
        self.dataSections = {}
        self.__findCodeSections(rom)
        self.__findDataSections(rom)
        
    def __findDataSections(self, rom):
        """Discovers underlying data sections (for use after code sections have been mapped)."""
        # Find all places where code sections are not defined.
        offset = 0
        while offset < len(rom.prgRom):
            address = NESMemory.offsetToPointer(offset)
            if(address in self.codeSections):
                # Skip over code section
                offset += self.codeSections[address].getSize()
            else:
                # Determine length of data section
                sectionStart = offset
                sectionSize = 0
                while sectionStart + sectionSize < len(rom.prgRom):
                    sectionSize += 1
                    if(NESMemory.offsetToPointer(sectionStart + sectionSize) in self.codeSections):
                        break
                dataSection = PRGROMDataSection(sectionStart, rom.prgRom[sectionStart:sectionStart+sectionSize])
                self.dataSections[dataSection.address] = dataSection
                offset += sectionSize
    
    def __findCodeSections(self, rom):
        """Discovers underlying code sections."""
        # We need to map code sections. Obtain our start address and NMI
        interruptVectorOffset = len(rom.prgRom) - 0x06
        self.interruptNMI = (rom.prgRom[interruptVectorOffset+1] << 8) | rom.prgRom[interruptVectorOffset]
        self.interruptReset = (rom.prgRom[interruptVectorOffset+3] << 8) | rom.prgRom[interruptVectorOffset+2]
        self.interruptIRQ = (rom.prgRom[interruptVectorOffset+5] << 8) | rom.prgRom[interruptVectorOffset+4]

        # Detect code sections recursively from here.
        self.__findCodeSectionsRec(rom, self.interruptNMI, interruptVectorOffset, PRGROMCodeSectionType.INTERRUPT)
        self.__findCodeSectionsRec(rom, self.interruptReset, interruptVectorOffset + 2, PRGROMCodeSectionType.INTERRUPT)
        self.__findCodeSectionsRec(rom, self.interruptIRQ, interruptVectorOffset + 4, PRGROMCodeSectionType.INTERRUPT)
        
        
    def __findCodeSectionsRec(self, rom, address, referenceType, referencedBy):
        """Recursively discovers code sections, starting with the given reference information."""
           
        # Obtain our offset relative to PRG-ROM start.
        offset = NESMemory.pointerToOffset(rom, address)
        address = NESMemory.offsetToPointer(offset) # corrects memory mirrored addresses
        
        #print("Entering: {}".format(hex(address)))
        
        # If we have a code section for this address, update the reference and stop
        if(address in self.codeSections):
            self.codeSections[address].addRef(referenceType, referencedBy)
            return
        
        # Check if we have a code section encompassing this address
        splitCodeSection = None
        for codeSection in self.codeSections.values():
            if(address > codeSection.address and address < codeSection.address + codeSection.getSize()):
                # Split the old code section (it's done), and continue by setting our current address/offset to the end of the new split code section.
                splitCodeSection = codeSection.split(offset)
                if(splitCodeSection != None):
                    address = splitCodeSection.address
                break
        
        # Either this is a new section or we encountered overlapping instructions.
        # We parse this as a new section until we detect an end.
        # (Overlapping instructions may re-parse some instructions that become aligned later twice)
        # (which we're okay with since adding advanced control flow logic for this would be difficult)
        codeSection = PRGROMCodeSection(offset) if splitCodeSection == None else splitCodeSection
        codeSection.addRef(referenceType, referencedBy)
        self.codeSections[address] = codeSection

        # Resume from where we left off.
        offset = codeSection.offset + codeSection.getSize()
        address = codeSection.address + codeSection.getSize()
        
        # Keep adding instructions while we arn't marked as done 
        # (split, or reached an instruction that marks end of control flow for this block)
        while offset < len(rom.prgRom) and not codeSection.done:
            # If we're at the start of a code section and it's not this one, stop
            if(address in self.codeSections and self.codeSections.get(address) != codeSection):
                codeSection.done = True
                break
            
            # Parse our instruction and increment offset
            instruction = MOSInstruction(rom.prgRom, offset)
            #print("@offset: {}".format(hex(NESMemory.offsetToPointer(offset))))
            #print("@instr: {}".format(str(instruction)))
            offset += instruction.definition.size
            address += instruction.definition.size
            codeSection.addInstruction(instruction)

            # Check our type which is used to determine code flow
            instrType = type(instruction.definition)
        
            # Check we have a jump
            jumpOffset = self.resolveJumpOffset(rom, instruction)
            if(jumpOffset != None):
                # Obtain address and add it as a function (JSR jump) or generic code location.
                jumpPointer = NESMemory.offsetToPointer(jumpOffset)
                referenceType = PRGROMCodeSectionType.FUNCTION if instrType is MOSInstr_JSR else PRGROMCodeSectionType.LOCATION
                self.__findCodeSectionsRec(rom, jumpPointer, referenceType, instruction.offset)

            # If our instruction is known to mark the end of a section, or our code section has now been split (done), or we reached another code section, stop.
            if(instruction.definition.marksEndOfSection):
                codeSection.done = True
                break
                
        return
    
    def resolveJumpOffset(self, rom, instruction):
        """Resolves the offset of a jump instruction's concluding jump."""
        if(instruction.definition.isJumpOrBranch):
            if(instruction.definition.mode == MOSAddressingMode.RELATIVE):
                return instruction.offset + instruction.definition.size + instruction.operand
            elif(instruction.definition.mode == MOSAddressingMode.ABSOLUTE):
                if(NESMemory.isROMMemory(instruction.operand)):
                    return NESMemory.pointerToOffset(rom, instruction.operand)
                else:
                    print("Warning: Absolute jump detected at address {} which jumps to an address outside of ROM memory at {}. If this is executed, it will cause an error.".format(hex(instruction.address, hex(instruction.operand))))
            elif(instruction.definition.mode == MOSAddressingMode.INDIRECT):
                print("Warning: Runtime calculated jump detected at address {} using indirect address {}. This section cannot be guaranteed to have been mapped and may cause an error.".format(hex(instruction.address), hex(instruction.operand)))
            else:
                raise ValueError("Unexpected jump mode when scanning code sections.")
        return None