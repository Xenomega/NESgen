import os
import fnmatch
import re
from shutil import copyfile
import time

if __name__ == "__main__":
	pythonExe = "python3.5"
	curdir = os.path.dirname(os.path.abspath(__file__))
	NESgen = os.path.join(curdir, "..", "..", "..", "NESgen", "NESgen", "NESgen.py")
	gameC = os.path.join(curdir, "..", "..", "..", "NESgen", "NESsys", "src", "game.c")
	gameH = os.path.join(curdir, "..", "..", "..", "NESgen", "NESsys", "src", "game.h")
	compiler = os.path.join(curdir, "..", "..", "..", "NESgen", "NESsys", "compile.sh")
	executable = os.path.join(curdir, "..", "..", "..", "NESgen", "NESsys", "bin", "NESsys")
	searchPattern = re.compile(fnmatch.translate("*.NES"), re.IGNORECASE)
	NESfiles = [name for name in os.listdir(curdir) if searchPattern.match(name)]
	for NESfile in NESfiles:
		# Create our output paths (old/new) = (no optimizations/optimizations)
		print("Compiling .c/.h: {}...".format(NESfile))
		gameCSrcDestOld = "{}_old.c".format(NESfile[:-4])
		gameCSrcDestOld = os.path.join(curdir, "src", gameCSrcDestOld)
		gameHSrcDestOld = "{}_old.h".format(NESfile[:-4])
		gameHSrcDestOld = os.path.join(curdir, "src", gameHSrcDestOld)
		gameCSrcDest = "{}_new.c".format(NESfile[:-4])
		gameCSrcDest = os.path.join(curdir, "src", gameCSrcDest)
		gameHSrcDest = "{}_new.h".format(NESfile[:-4])
		gameHSrcDest = os.path.join(curdir, "src", gameHSrcDest)
		outputName = "{}_binary".format(NESfile[:-4])
		NESfile = os.path.join(curdir, NESfile)
		outputPath = os.path.join(curdir, "bin", outputName)

		# Output old files (optimizations disabled)
		command = """{} {} -i "{}" -c "{}" -h "{}" -n""".format(pythonExe, NESgen, NESfile, gameC, gameH)
		print(command)		
		result = os.system(command) >> 8
		if(result != 0):
			print("An error occured in ROM: ", NESfile)
			continue
		print("Waiting...")
		time.sleep(0.5)
		print("Copying source files into Test Suite /src/ folder as well...")
		copyfile(gameC, gameCSrcDestOld)
		copyfile(gameH, gameHSrcDestOld)

		# Output new files (optimizations enabled)
		command = """{} {} -i "{}" -c "{}" -h "{}" -f""".format(pythonExe, NESgen, NESfile, gameC, gameH)
		result = os.system(command) >> 8
		if(result != 0):
			print("An error occured in ROM: ", NESfile)
			continue
		print("Waiting...")
		time.sleep(0.5)
		print("Copying source files into Test Suite /src/ folder as well...")
		copyfile(gameC, gameCSrcDest)
		copyfile(gameH, gameHSrcDest)

		# Compile executable (optimizations enabled)
		print("Compiling executable: {}...".format(outputName))
		os.system(compiler)
		print("Waiting...")
		time.sleep(0.5)
		print("Copying executable: {}...".format(outputName))
		copyfile(executable, outputPath)
		time.sleep(0.5)
		print("Setting permissions: {}...".format(outputName))
		command = "chmod +x {}".format(outputPath)
		os.system(command)
	print("Operation complete.")

