import os
import glob

if __name__ == "__main__":
	pythonExe = "python3.5"
	curdir = os.path.realpath('.')
	NESgen = os.path.join(curdir, "..", "..", "..", "NESgen/NESgen/NESgen.py")
	NESfiles = glob.glob("*.NES")
	for NESfile in NESfiles:
		print("Parsed: " + NESfile)
		NESfile = os.path.join(curdir, NESfile)
		outputFile = NESfile[:-4] + "_Parsed.ASM"
		command = pythonExe + " " + NESgen + " " + NESfile + " > " + outputFile
		os.system(command)
	print("Operation complete.")
