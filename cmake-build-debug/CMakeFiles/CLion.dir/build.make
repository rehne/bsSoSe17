# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Volumes/VOLUME/Applications/CLion.app/Contents/bin/cmake/bin/cmake

# The command to remove a file.
RM = /Volumes/VOLUME/Applications/CLion.app/Contents/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/CLion.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CLion.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CLion.dir/flags.make

CMakeFiles/CLion.dir/server.c.o: CMakeFiles/CLion.dir/flags.make
CMakeFiles/CLion.dir/server.c.o: ../server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/CLion.dir/server.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/CLion.dir/server.c.o   -c /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17/server.c

CMakeFiles/CLion.dir/server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CLion.dir/server.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17/server.c > CMakeFiles/CLion.dir/server.c.i

CMakeFiles/CLion.dir/server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CLion.dir/server.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17/server.c -o CMakeFiles/CLion.dir/server.c.s

CMakeFiles/CLion.dir/server.c.o.requires:

.PHONY : CMakeFiles/CLion.dir/server.c.o.requires

CMakeFiles/CLion.dir/server.c.o.provides: CMakeFiles/CLion.dir/server.c.o.requires
	$(MAKE) -f CMakeFiles/CLion.dir/build.make CMakeFiles/CLion.dir/server.c.o.provides.build
.PHONY : CMakeFiles/CLion.dir/server.c.o.provides

CMakeFiles/CLion.dir/server.c.o.provides.build: CMakeFiles/CLion.dir/server.c.o


# Object files for target CLion
CLion_OBJECTS = \
"CMakeFiles/CLion.dir/server.c.o"

# External object files for target CLion
CLion_EXTERNAL_OBJECTS =

CLion: CMakeFiles/CLion.dir/server.c.o
CLion: CMakeFiles/CLion.dir/build.make
CLion: CMakeFiles/CLion.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable CLion"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CLion.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CLion.dir/build: CLion

.PHONY : CMakeFiles/CLion.dir/build

CMakeFiles/CLion.dir/requires: CMakeFiles/CLion.dir/server.c.o.requires

.PHONY : CMakeFiles/CLion.dir/requires

CMakeFiles/CLion.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CLion.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CLion.dir/clean

CMakeFiles/CLion.dir/depend:
	cd /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17 /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17 /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17/cmake-build-debug /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17/cmake-build-debug /Volumes/VOLUME/sciebo/Semester6/BS/Praktikum/bsSoSe17/cmake-build-debug/CMakeFiles/CLion.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CLion.dir/depend
