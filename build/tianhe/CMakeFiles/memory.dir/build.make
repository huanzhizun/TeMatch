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
include tianhe/CMakeFiles/memory.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tianhe/CMakeFiles/memory.dir/compiler_depend.make

# Include the progress variables for this target.
include tianhe/CMakeFiles/memory.dir/progress.make

# Include the compile flags for this target's objects.
include tianhe/CMakeFiles/memory.dir/flags.make

tianhe/CMakeFiles/memory.dir/tcs_memory.cpp.o: tianhe/CMakeFiles/memory.dir/flags.make
tianhe/CMakeFiles/memory.dir/tcs_memory.cpp.o: /root/jiang/DTGraph_bak/tianhe/tcs_memory.cpp
tianhe/CMakeFiles/memory.dir/tcs_memory.cpp.o: tianhe/CMakeFiles/memory.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/jiang/DTGraph_bak/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tianhe/CMakeFiles/memory.dir/tcs_memory.cpp.o"
	cd /root/jiang/DTGraph_bak/build/tianhe && mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tianhe/CMakeFiles/memory.dir/tcs_memory.cpp.o -MF CMakeFiles/memory.dir/tcs_memory.cpp.o.d -o CMakeFiles/memory.dir/tcs_memory.cpp.o -c /root/jiang/DTGraph_bak/tianhe/tcs_memory.cpp

tianhe/CMakeFiles/memory.dir/tcs_memory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/memory.dir/tcs_memory.cpp.i"
	cd /root/jiang/DTGraph_bak/build/tianhe && mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/jiang/DTGraph_bak/tianhe/tcs_memory.cpp > CMakeFiles/memory.dir/tcs_memory.cpp.i

tianhe/CMakeFiles/memory.dir/tcs_memory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/memory.dir/tcs_memory.cpp.s"
	cd /root/jiang/DTGraph_bak/build/tianhe && mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/jiang/DTGraph_bak/tianhe/tcs_memory.cpp -o CMakeFiles/memory.dir/tcs_memory.cpp.s

# Object files for target memory
memory_OBJECTS = \
"CMakeFiles/memory.dir/tcs_memory.cpp.o"

# External object files for target memory
memory_EXTERNAL_OBJECTS =

bin/memory: tianhe/CMakeFiles/memory.dir/tcs_memory.cpp.o
bin/memory: tianhe/CMakeFiles/memory.dir/build.make
bin/memory: libs/libgraph_mining_without_thread_local.so
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_cord.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_cordz_info.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_cord_internal.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_cordz_functions.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_cordz_handle.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_crc_cord_state.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_crc32c.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_crc_internal.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_crc_cpu_detect.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_str_format_internal.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_raw_hash_set.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_hash.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_city.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_bad_variant_access.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_low_level_hash.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_bad_optional_access.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_hashtablez_sampler.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_exponential_biased.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_synchronization.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_stacktrace.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_graphcycles_internal.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_kernel_timeout_internal.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_time.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_civil_time.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_time_zone.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_symbolize.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_strings.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_int128.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_string_view.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_strings_internal.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_debugging_internal.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_malloc_internal.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_demangle_internal.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_base.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_spinlock_wait.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_throw_delegate.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_raw_logging_internal.a
bin/memory: vcpkg_installed/x64-linux/lib/libabsl_log_severity.a
bin/memory: tianhe/CMakeFiles/memory.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/root/jiang/DTGraph_bak/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/memory"
	cd /root/jiang/DTGraph_bak/build/tianhe && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/memory.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tianhe/CMakeFiles/memory.dir/build: bin/memory
.PHONY : tianhe/CMakeFiles/memory.dir/build

tianhe/CMakeFiles/memory.dir/clean:
	cd /root/jiang/DTGraph_bak/build/tianhe && $(CMAKE_COMMAND) -P CMakeFiles/memory.dir/cmake_clean.cmake
.PHONY : tianhe/CMakeFiles/memory.dir/clean

tianhe/CMakeFiles/memory.dir/depend:
	cd /root/jiang/DTGraph_bak/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/jiang/DTGraph_bak /root/jiang/DTGraph_bak/tianhe /root/jiang/DTGraph_bak/build /root/jiang/DTGraph_bak/build/tianhe /root/jiang/DTGraph_bak/build/tianhe/CMakeFiles/memory.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : tianhe/CMakeFiles/memory.dir/depend

