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
include tianhe/CMakeFiles/baseline.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tianhe/CMakeFiles/baseline.dir/compiler_depend.make

# Include the progress variables for this target.
include tianhe/CMakeFiles/baseline.dir/progress.make

# Include the compile flags for this target's objects.
include tianhe/CMakeFiles/baseline.dir/flags.make

tianhe/CMakeFiles/baseline.dir/baseline_test.cpp.o: tianhe/CMakeFiles/baseline.dir/flags.make
tianhe/CMakeFiles/baseline.dir/baseline_test.cpp.o: /root/jiang/DTGraph_bak/tianhe/baseline_test.cpp
tianhe/CMakeFiles/baseline.dir/baseline_test.cpp.o: tianhe/CMakeFiles/baseline.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/jiang/DTGraph_bak/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tianhe/CMakeFiles/baseline.dir/baseline_test.cpp.o"
	cd /root/jiang/DTGraph_bak/build/tianhe && mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tianhe/CMakeFiles/baseline.dir/baseline_test.cpp.o -MF CMakeFiles/baseline.dir/baseline_test.cpp.o.d -o CMakeFiles/baseline.dir/baseline_test.cpp.o -c /root/jiang/DTGraph_bak/tianhe/baseline_test.cpp

tianhe/CMakeFiles/baseline.dir/baseline_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/baseline.dir/baseline_test.cpp.i"
	cd /root/jiang/DTGraph_bak/build/tianhe && mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/jiang/DTGraph_bak/tianhe/baseline_test.cpp > CMakeFiles/baseline.dir/baseline_test.cpp.i

tianhe/CMakeFiles/baseline.dir/baseline_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/baseline.dir/baseline_test.cpp.s"
	cd /root/jiang/DTGraph_bak/build/tianhe && mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/jiang/DTGraph_bak/tianhe/baseline_test.cpp -o CMakeFiles/baseline.dir/baseline_test.cpp.s

# Object files for target baseline
baseline_OBJECTS = \
"CMakeFiles/baseline.dir/baseline_test.cpp.o"

# External object files for target baseline
baseline_EXTERNAL_OBJECTS =

bin/baseline: tianhe/CMakeFiles/baseline.dir/baseline_test.cpp.o
bin/baseline: tianhe/CMakeFiles/baseline.dir/build.make
bin/baseline: libs/libgraph_mining_without_thread_local.so
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_cord.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_cordz_info.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_cord_internal.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_cordz_functions.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_cordz_handle.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_crc_cord_state.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_crc32c.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_crc_internal.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_crc_cpu_detect.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_str_format_internal.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_raw_hash_set.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_hash.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_city.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_bad_variant_access.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_low_level_hash.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_bad_optional_access.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_hashtablez_sampler.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_exponential_biased.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_synchronization.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_stacktrace.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_graphcycles_internal.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_kernel_timeout_internal.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_time.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_civil_time.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_time_zone.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_symbolize.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_strings.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_int128.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_string_view.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_strings_internal.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_debugging_internal.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_malloc_internal.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_demangle_internal.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_base.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_spinlock_wait.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_throw_delegate.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_raw_logging_internal.a
bin/baseline: vcpkg_installed/x64-linux/lib/libabsl_log_severity.a
bin/baseline: tianhe/CMakeFiles/baseline.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/root/jiang/DTGraph_bak/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/baseline"
	cd /root/jiang/DTGraph_bak/build/tianhe && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/baseline.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tianhe/CMakeFiles/baseline.dir/build: bin/baseline
.PHONY : tianhe/CMakeFiles/baseline.dir/build

tianhe/CMakeFiles/baseline.dir/clean:
	cd /root/jiang/DTGraph_bak/build/tianhe && $(CMAKE_COMMAND) -P CMakeFiles/baseline.dir/cmake_clean.cmake
.PHONY : tianhe/CMakeFiles/baseline.dir/clean

tianhe/CMakeFiles/baseline.dir/depend:
	cd /root/jiang/DTGraph_bak/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/jiang/DTGraph_bak /root/jiang/DTGraph_bak/tianhe /root/jiang/DTGraph_bak/build /root/jiang/DTGraph_bak/build/tianhe /root/jiang/DTGraph_bak/build/tianhe/CMakeFiles/baseline.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : tianhe/CMakeFiles/baseline.dir/depend

