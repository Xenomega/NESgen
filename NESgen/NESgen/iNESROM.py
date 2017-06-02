# iNES ROM Support
# Reference: https://wiki.nesdev.com/w/index.php/INES
from enum import Enum
import struct
class iNESROM:
    INES_HEADER_SIGNATURE = 0x4e45531a # "NES" + MS-DOS EOF
    PRG_ROM_BANK_SIZE = 0x4000
    CHR_ROM_BANK_SIZE = 0x2000
    PRG_RAM_BANK_SIZE = 0x2000
    def __init__(self, romData):
        """Parses iNES ROM format given an array of bytes."""
        assert romData != None and len(romData) >= 0x10, "iNES: invalid file size"
        
        # Unpack basic header values
        headerSignature = struct.unpack(">I", romData[0:4])[0]
        prgRomSize = romData[4] * self.PRG_ROM_BANK_SIZE #  count of 16kb blocks
        chrRomSize = romData[5] * self.CHR_ROM_BANK_SIZE # count of 8kb blocks
        flags6 = romData[6]
        flags7 = romData[7]
        zeroFilled = struct.unpack("I", romData[12:16])
        assert headerSignature == self.INES_HEADER_SIGNATURE, "iNES: header signature mismatch"
        
    
        # Extract flags
        self.mirroring = self.MirroringType(flags6 & (1 << 0))
        self.hasPersistentRam = bool(flags6 & (1 << 1))
        self.hasTrainer = bool(flags6 & (1 << 2))
        self.ignoreMirroring = bool(flags6 & (1 << 3)) # ignores mirroring, uses 4-screen vram.
        if(self.ignoreMirroring):
            self.mirroring = self.MirroringType.FOUR_SCREEN

        # To continue, we'll need to know ROM type. Our wiki explains this.
        nes2Signature = flags7 & 0x0C
        if(nes2Signature == 0x08): # should check ROM size is valid as well.
            self.romType = self.ROMType.NES2
        elif(nes2Signature == 0x00 and zeroFilled == 0):
            self.romType = self.ROMType.INES
        else:
            self.romType = self.ROMType.ARCHAIC

        # Read critical values depending on our format.
        self.mappingNumber = (flags6 >> 4)
        if(self.romType == self.ROMType.ARCHAIC):
            self.prgRamSize = self.PRG_RAM_BANK_SIZE
        elif(self.romType == self.ROMType.INES):
            self.prgRamSize = max(romData[8], 1) * self.PRG_RAM_BANK_SIZE # count of 8kb blocks, at least 1.
        elif(self.romType == self.ROMType.NES2):
            self.mappingNumber |= (flags7 & 0xF0)
        
        assert self.romType != self.ROMType.NES2, "iNES: NES2.0 currently unsupported."
        
        # Finally, copy our Trainer and ROM data out.
        offset = 0x10
        if(self.hasTrainer):
            self.trainer = romData[offset:offset+0x200]
        else:
            self.trainer = []
        offset += len(self.trainer)
        self.prgRom = romData[offset:offset+prgRomSize]
        offset += prgRomSize
        self.chrRom = romData[offset:offset+chrRomSize]
        
    def printRomInfo(self):
        """Prints generic iNES ROM header information"""
        print("iNES ROM Information:")
        print("---------------------")
        print("ROM Type:", self.romType)
        print("PRG-ROM Size:", hex(len(self.prgRom)))
        print("PRG-RAM Size:", hex(self.prgRamSize))
        print("CHR-ROM Size:", hex(len(self.chrRom)))
        print("Has Trainer:", self.hasTrainer)
        print("Has Persistent PRG-RAM:", self.hasPersistentRam)
        print("Mirroring: ", self.mirroring)
        print("Ignores Mirroring:", self.ignoreMirroring)
        print("Mapping Number:", self.mappingNumber)
        print("---------------------")
        
    # ROM Type
    class ROMType(Enum):
        """Describes the type of ROM we are parsing. Older formats have simple backwards compatibility."""
        ARCHAIC = 0
        INES = 1
        NES2 = 2 
        
    # Mirroring type used in flags for the PPU.
    class MirroringType(Enum):
        """Determines the way the PPU handles nametable mirroring."""
        HORIZONTAL = 0
        VERTICAL = 1
        FOUR_SCREEN = 2