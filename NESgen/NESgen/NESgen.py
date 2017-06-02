'''
NESgen - A NES->C compiler.
@author: David Pokora
'''
from iNESROM import iNESROM
from iNESROMDisassembler import iNESROMDisassembler
import getopt, sys

def usage():
	"""Prints the usage for the application"""
	print("NESgen.py [-n] -i <input.NES> -c <game.c> -h <game.h>")
	print("Options:")
	print("-i")
	print("\tInput path for the .NES ROM file.")
	print("-c")
	print("\tOutput path for the C source file.")
	print("-h")
	print("\tOutput path for the C header file.")
	print("-n")
	print("\tNo readability-based optimizations enabled.")
	print("-f")
	print("\tMake full PRG-ROM data available, instead of just predicted data sections.")

if __name__ == "__main__":
	# For debugging:
	#sys.argv = [0, '/home/nx/Desktop/testROMs/Flame.NES', '/home/nx/488_nes2c/NESgen/NESsys/src/game.c', '/home/nx/488_nes2c/NESgen/NESsys/src/game.h']

	# Attempt to obtain our options/arguments.
	try:
		opts, args = getopt.getopt(sys.argv[1:],"i:c:h:fn",[])
	except getopt.GetoptError as err:
		print(err)
		usage()
		sys.exit(2)
		
	romPath = None
	sourcePath = None
	headerPath = None
	for opt, arg in opts:
		if opt == "-i":
			# Set ROM input path.
			romPath = arg
		elif opt == "-c":
			# Set source output path
			sourcePath = arg
		elif opt == "-h":
			# Set header output path
			headerPath = arg
		elif opt == "-f":
			# Provide data access to entire PRG-ROM instead of just determined data sections
			iNESROMDisassembler.OUTPUT_FULL_PRGROM_DATA = True
		elif opt == "-n":
			# Marked as "no optimizations"
			iNESROMDisassembler.ALLOW_FUNCTION_NAME_OVERRIDES = False
			iNESROMDisassembler.ALLOW_KNOWN_MEMORY_ACCESS_LABELS = False
			iNESROMDisassembler.ALLOW_RUNTIME_LOCATIONS = False
			
	# Verify we were passed valid arguments.
	if(romPath is None or sourcePath is None or headerPath is None):
		usage()
		sys.exit(2)
		
	# Open our test ROM
	with open(romPath, mode='rb') as file:
		romData = file.read()
		rom = iNESROM(romData)
	# Print any information regarding the ROM.
	#rom.printRomInfo()
	# Disassemble it.
	disasm = iNESROMDisassembler()
	#disasm.DisassembleToASM(rom)
	disasm.DisassembleToC(rom, sourcePath, headerPath)