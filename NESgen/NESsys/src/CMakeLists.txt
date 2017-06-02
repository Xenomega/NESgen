# Define our project name.
cmake_minimum_required(VERSION 2.6)
set(APPNAME NESsys)
project(${APPNAME})

# Overwrite output directories to ensure we always output to bin (MSVC tries to use it's paths, undesirable)
set(OUTDIR ${CMAKE_SOURCE_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTDIR})
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTDIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTDIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTDIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)


# Configure our input files to be compiled.
file(GLOB SRC_FILES
	"*.h"
	"*.c"
)
add_executable(${APPNAME} ${SRC_FILES})

# Now OpenGL just needs to be setup per OS.
if(WIN32)
	# Windows Compilation
	target_link_libraries(${APPNAME} freeglut.lib)
elseif(APPLE)
	# Mac OS Compilation
	find_library(GLUT_LIBRARY GLUT)
	find_library(OpenGL_LIBRARY OpenGL)
	mark_as_advanced(GLUT_LIBRARY OpenGL_LIBRARY)
	target_link_libraries(${APPNAME} ${GLUT_LIBRARY} ${OpenGL_LIBRARY})
else()
	# Linux/Other Compilation
	target_link_libraries(${APPNAME} GL GLU glut pthread)
endif()

# Keep debug info if using GCC.
IF(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
