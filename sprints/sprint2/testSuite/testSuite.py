import os
import glob
from shutil import copyfile
import time

if __name__ == "__main__":
	pythonExe = "python3.5"
	curdir = os.path.realpath('.')
	NESgen = os.path.join(curdir, "..", "..", "..", "NESgen", "NESgen", "NESgen.py")
	gameC = os.path.join(curdir, "..", "..", "..", "NESgen", "NESsys", "src", "game.c")
	gameH = os.path.join(curdir, "..", "..", "..", "NESgen", "NESsys", "src", "game.h")
	compiler = os.path.join(curdir, "..", "..", "..", "NESgen", "NESsys", "compile.sh")
	executable = os.path.join(curdir, "..", "..", "..", "NESgen", "NESsys", "bin", "NESsys")
	NESfiles = glob.glob("*.NES")
	for NESfile in NESfiles:
		print("Compiling .c/.h: {}...".format(NESfile))
		gameCSrcDest = "{}_game.c".format(NESfile[:-4])
		gameCSrcDest = os.path.join(curdir, "src", gameCSrcDest)
		gameHSrcDest = "{}_game.h".format(NESfile[:-4])
		gameHSrcDest = os.path.join(curdir, "src", gameHSrcDest)
		outputName = "{}_binary".format(NESfile[:-4])
		NESfile = os.path.join(curdir, NESfile)
		outputPath = os.path.join(curdir, "bin", outputName)
		command = "{} {} {} {} {}".format(pythonExe, NESgen, NESfile, gameC, gameH)
		os.system(command)
		print("Waiting...")
		time.sleep(0.5)
		print("Copying source files into Test Suite /src/ folder as well...")
		copyfile(gameC, gameCSrcDest)
		copyfile(gameH, gameHSrcDest)
		print("Compiling executable: {}...".format(outputName))
		os.system(compiler)
		print("Waiting...")
		time.sleep(0.5)
		print("Copying executable: {}...".format(outputName))
		copyfile(executable, outputPath)
		print("Setting permissions: {}...".format(outputName))
		command = "chmod +x {}".format(outputPath)
		os.system(command)
	print("Operation complete.")

