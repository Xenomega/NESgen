#!/bin/sh

# Define our build and source directories
CURDIR=$(dirname "$0")
cd "$CURDIR"
BUILDDIR=$PWD/build
SRCDIR=$PWD/src

# Verify our build directory exists
mkdir -p "$BUILDDIR"

# Handle arguments accordingly.
if [ "$1" = "-c" ]; then
	# Clean, delete whole build directory.
	rm -rf "$BUILDDIR/"
else
	# Enter the build directory so that CMake build files are output there.
	cd "$BUILDDIR"

	# Set up the project to build (generates project files in bin)
	cmake -DCMAKE_BUILD_TYPE=Debug "$SRCDIR"

	# Build the project.
	cmake --build "$BUILDDIR"
fi




