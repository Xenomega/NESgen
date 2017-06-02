@echo off

REM Backup our current location.
SET OLDCD=%CD%

REM Switch to the scripts directory.
CD /D "%0\.."

REM Our source/build paths are relative from here.
SET BUILDDIR=%CD%\build
SET SRCDIR=%CD%\src"

REM Verify the build directory exists
IF NOT EXIST "%BUILDDIR%" MKDIR "%BUILDDIR%"

REM Handle arguments accordingly.
IF "%1"=="-c" (
	REM Clean, delete whole build directory
	RMDIR /Q /S "%BUILDDIR%"
) ELSE ( 
	
	REM We are building.

	REM Enter the build directory so that CMake build files are output there.
	CD /D "%BUILDDIR%"

	REM Set up the project to build (generates project files in bin)
	cmake "%SRCDIR%"

	REM Build the project.
	cmake --build "%BUILDDIR%"
)

REM Return back to our original directory
CD /D "%OLDCD%"

@echo on
