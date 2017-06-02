class NESMemory:
    """Provides various NES memory related functions."""
    MEMORY_START_ADDR = 0x0000
    MEMORY_END_ADDR = 0x10000
    PRG_ROM_START_ADDR = 0x8000
    PRG_ROM_END_ADDR = MEMORY_END_ADDR
    PRG_ROM_FIRST_BANK_ADDR = PRG_ROM_START_ADDR
    PRG_ROM_SECOND_BANK_ADDR = 0xC000
    
    @staticmethod
    def isROMMemory(addr):
        """Determines if the given address is within the PRG-ROM memory scope."""
        return addr >= NESMemory.PRG_ROM_START_ADDR and addr < NESMemory.PRG_ROM_END_ADDR
    
    @staticmethod
    def isMirroredROM(rom):
        """Determines if the given PRG-ROM is 8KB (mirrored) or 16KB (not)"""
        return len(rom.prgRom) == rom.PRG_ROM_BANK_SIZE
    
    @staticmethod
    def pointerToOffset(rom, memAddr):
        """Obtains an offset relative to the start of PRG-ROM from a PRG-ROM memory address."""
        assert memAddr >= NESMemory.PRG_ROM_START_ADDR and memAddr < NESMemory.PRG_ROM_END_ADDR, "Given PRG-ROM memory address not valid."
        # If we are accessing memory in the second bank but our game is only a single one, note it's mirrored so translate to beginning of ROM.
        if(memAddr >= NESMemory.PRG_ROM_SECOND_BANK_ADDR and NESMemory.isMirroredROM(rom)):
            return memAddr - NESMemory.PRG_ROM_SECOND_BANK_ADDR
        else:
            return memAddr - NESMemory.PRG_ROM_START_ADDR
        
    @staticmethod
    def offsetToPointer(offset):
        """Obtains a PRG-ROM memory address from an offset relative to the start of PRG-ROM."""
        return offset + NESMemory.PRG_ROM_START_ADDR