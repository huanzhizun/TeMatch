# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /root/jiang/cmake-3.28.0-rc3-linux-x86_64/bin/cmake

# The command to remove a file.
RM = /root/jiang/cmake-3.28.0-rc3-linux-x86_64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/jiang/DTGraph_bak

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/jiang/DTGraph_bak/build

# Include any dependencies generated for this target.
include tianhe/CMakeFiles/unit_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tianhe/CMakeFiles/unit_test.dir/compiler_depend.make

# Include the progress variables for this target.
include tianhe/CMakeFiles/unit_test.dir/progress.make

# Include the compile flags for this target's objects.
include tianhe/CMakeFiles/unit_test.dir/flags.make

tianhe/CMakeFiles/unit_test.dir/unitTest.cpp.o: tianhe/CMakeFiles/unit_test.dir/flags.make
tianhe/CMakeFiles/unit_test.dir/unitTest.cpp.o: /root/jiang/DTGraph_bak/tianhe/unitTest.cpp
tianhe/CMakeFiles/unit_test.dir/unitTest.cpp.o: tianhe/CMakeFiles/unit_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/jiang/DTGraph_bak/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tianhe/CMakeFiles/unit_test.dir/unitTest.cpp.o"
	cd /root/jiang/DTGraph_bak/build/tianhe && mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tianhe/CMakeFiles/unit_test.dir/unitTest.cpp.o -MF CMakeFiles/unit_test.dir/unitTest.cpp.o.d -o CMakeFiles/unit_test.dir/unitTest.cpp.o -c /root/jiang/DTGraph_bak/tianhe/unitTest.cpp

tianhe/CMakeFiles/unit_test.dir/unitTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/unit_test.dir/unitTest.cpp.i"
	cd /root/jiang/DTGraph_bak/build/tianhe && mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/jiang/DTGraph_bak/tianhe/unitTest.cpp > CMakeFiles/unit_test.dir/unitTest.cpp.i

tianhe/CMakeFiles/unit_test.dir/unitTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/unit_test.dir/unitTest.cpp.s"
	cd /root/jiang/DTGraph_bak/build/tianhe && mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/jiang/DTGraph_bak/tianhe/unitTest.cpp -o CMakeFiles/unit_test.dir/unitTest.cpp.s

# Object files for target unit_test
unit_test_OBJECTS = \
"CMakeFiles/unit_test.dir/unitTest.cpp.o"

# External object files for target unit_test
unit_test_EXTERNAL_OBJECTS =

bin/unit_test: tianhe/CMakeFiles/unit_test.dir/unitTest.cpp.o
bin/unit_test: tianhe/CMakeFiles/unit_test.dir/build.make
bin/unit_test: libs/libgraph_mining_without_thread_local.so
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_cord.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_cordz_info.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_cord_internal.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_cordz_functions.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_cordz_handle.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_crc_cord_state.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_crc32c.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_crc_internal.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_crc_cpu_detect.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_str_format_internal.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_raw_hash_set.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_hash.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_city.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_bad_variant_access.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_low_level_hash.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_bad_optional_access.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_hashtablez_sampler.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_exponential_biased.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_synchronization.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_stacktrace.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_graphcycles_internal.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_kernel_timeout_internal.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_time.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_civil_time.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_time_zone.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_symbolize.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_strings.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_int128.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_string_view.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_strings_internal.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_debugging_internal.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_malloc_internal.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_demangle_internal.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_base.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_spinlock_wait.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_throw_delegate.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_raw_logging_internal.a
bin/unit_test: vcpkg_installed/x64-linux/lib/libabsl_log_severity.a
bin/unit_test: tianhe/CMakeFiles/unit_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/root/jiang/DTGraph_bak/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/unit_test"
	cd /root/jiang/DTGraph_bak/build/tianhe && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/unit_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tianhe/CMakeFiles/unit_test.dir/build: bin/unit_test
.PHONY : tianhe/CMakeFiles/unit_test.dir/build

tianhe/CMakeFiles/unit_test.dir/clean:
	cd /root/jiang/DTGraph_bak/build/tianhe && $(CMAKE_COMMAND) -P CMakeFiles/unit_test.dir/cmake_clean.cmake
.PHONY : tianhe/CMakeFiles/unit_test.dir/clean

tianhe/CMakeFiles/unit_test.dir/depend:
	cd /root/jiang/DTGraph_bak/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/jiang/DTGraph_bak /root/jiang/DTGraph_bak/tianhe /root/jiang/DTGraph_bak/build /root/jiang/DTGraph_bak/build/tianhe /root/jiang/DTGraph_bak/build/tianhe/CMakeFiles/unit_test.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : tianhe/CMakeFiles/unit_test.dir/depend

