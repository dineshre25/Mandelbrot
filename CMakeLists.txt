cmake_minimum_required(VERSION 3.8...4.1)

# *******************************************
# ************* CMake Content ***************
# *******************************************
# This CMake create a workspace containing the following projects
# 
# Programs
#  - mandelbrot

project(mandelbrot)

# Profiling option.
option(ENABLE_PROFILING "Enable profiling with GCC code instrumentation." OFF)
if(ENABLE_PROFILING AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
	MESSAGE("Enable gprof profiling for GNU/MinGW Debug builds")
	add_definitions( -DENABLE_PROFILING )
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-pie -g -finstrument-functions")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
	if(WIN32)
		# Keep traced function pointers in the executable's link-time address range.
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-dynamicbase")
	endif()
endif()

# Add definition for relative path into project
add_definitions( -DPROJECT_ROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			MESSAGE("Generate Debug project")
			set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
			set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall")
	else()
			MESSAGE("Generate Release project")
			set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
			set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall")
	endif()
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# *******************************************
# **************** Libraries ****************
# *******************************************

set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake_modules/")
include(${LIBS_DIR}/CMakeLists.txt)

# *******************************************
# ************** Dat files ******************
# *******************************************

set(DAT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dat)
include(${DAT_DIR}/CMakeLists.txt)

# *******************************************
# ************ mandelbrot.exe  *************
# *******************************************
find_package(OpenMP REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SDL2_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR})
file(
	GLOB_RECURSE
	source_files
	./src/*.c
	./include/*.h
)

add_executable(mandelbrot ${source_files} ${header_files})

if (WIN32)
	target_link_libraries(mandelbrot ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} OpenMP::OpenMP_C)
else()
	target_link_libraries(mandelbrot ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} m OpenMP::OpenMP_C)
endif()