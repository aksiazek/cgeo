# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /media/ark/Data/Code/Dev/Courses/Geom/gogui/example

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/ark/Data/Code/Dev/Courses/Geom/gogui/example

# Include any dependencies generated for this target.
include cppjson/CMakeFiles/cppjson.dir/depend.make

# Include the progress variables for this target.
include cppjson/CMakeFiles/cppjson.dir/progress.make

# Include the compile flags for this target's objects.
include cppjson/CMakeFiles/cppjson.dir/flags.make

cppjson/CMakeFiles/cppjson.dir/json.cc.o: cppjson/CMakeFiles/cppjson.dir/flags.make
cppjson/CMakeFiles/cppjson.dir/json.cc.o: /media/ark/Data/Code/Dev/Courses/Geom/gogui/cppjson/json.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /media/ark/Data/Code/Dev/Courses/Geom/gogui/example/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object cppjson/CMakeFiles/cppjson.dir/json.cc.o"
	cd /media/ark/Data/Code/Dev/Courses/Geom/gogui/example/cppjson && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/cppjson.dir/json.cc.o -c /media/ark/Data/Code/Dev/Courses/Geom/gogui/cppjson/json.cc

cppjson/CMakeFiles/cppjson.dir/json.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cppjson.dir/json.cc.i"
	cd /media/ark/Data/Code/Dev/Courses/Geom/gogui/example/cppjson && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /media/ark/Data/Code/Dev/Courses/Geom/gogui/cppjson/json.cc > CMakeFiles/cppjson.dir/json.cc.i

cppjson/CMakeFiles/cppjson.dir/json.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cppjson.dir/json.cc.s"
	cd /media/ark/Data/Code/Dev/Courses/Geom/gogui/example/cppjson && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /media/ark/Data/Code/Dev/Courses/Geom/gogui/cppjson/json.cc -o CMakeFiles/cppjson.dir/json.cc.s

cppjson/CMakeFiles/cppjson.dir/json.cc.o.requires:
.PHONY : cppjson/CMakeFiles/cppjson.dir/json.cc.o.requires

cppjson/CMakeFiles/cppjson.dir/json.cc.o.provides: cppjson/CMakeFiles/cppjson.dir/json.cc.o.requires
	$(MAKE) -f cppjson/CMakeFiles/cppjson.dir/build.make cppjson/CMakeFiles/cppjson.dir/json.cc.o.provides.build
.PHONY : cppjson/CMakeFiles/cppjson.dir/json.cc.o.provides

cppjson/CMakeFiles/cppjson.dir/json.cc.o.provides.build: cppjson/CMakeFiles/cppjson.dir/json.cc.o

# Object files for target cppjson
cppjson_OBJECTS = \
"CMakeFiles/cppjson.dir/json.cc.o"

# External object files for target cppjson
cppjson_EXTERNAL_OBJECTS =

cppjson/libcppjson.a: cppjson/CMakeFiles/cppjson.dir/json.cc.o
cppjson/libcppjson.a: cppjson/CMakeFiles/cppjson.dir/build.make
cppjson/libcppjson.a: cppjson/CMakeFiles/cppjson.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libcppjson.a"
	cd /media/ark/Data/Code/Dev/Courses/Geom/gogui/example/cppjson && $(CMAKE_COMMAND) -P CMakeFiles/cppjson.dir/cmake_clean_target.cmake
	cd /media/ark/Data/Code/Dev/Courses/Geom/gogui/example/cppjson && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cppjson.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
cppjson/CMakeFiles/cppjson.dir/build: cppjson/libcppjson.a
.PHONY : cppjson/CMakeFiles/cppjson.dir/build

cppjson/CMakeFiles/cppjson.dir/requires: cppjson/CMakeFiles/cppjson.dir/json.cc.o.requires
.PHONY : cppjson/CMakeFiles/cppjson.dir/requires

cppjson/CMakeFiles/cppjson.dir/clean:
	cd /media/ark/Data/Code/Dev/Courses/Geom/gogui/example/cppjson && $(CMAKE_COMMAND) -P CMakeFiles/cppjson.dir/cmake_clean.cmake
.PHONY : cppjson/CMakeFiles/cppjson.dir/clean

cppjson/CMakeFiles/cppjson.dir/depend:
	cd /media/ark/Data/Code/Dev/Courses/Geom/gogui/example && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/ark/Data/Code/Dev/Courses/Geom/gogui/example /media/ark/Data/Code/Dev/Courses/Geom/gogui/cppjson /media/ark/Data/Code/Dev/Courses/Geom/gogui/example /media/ark/Data/Code/Dev/Courses/Geom/gogui/example/cppjson /media/ark/Data/Code/Dev/Courses/Geom/gogui/example/cppjson/CMakeFiles/cppjson.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : cppjson/CMakeFiles/cppjson.dir/depend
