CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
set(VCPKG_ROOT $ENV{VCPKG_ROOT})

if(NOT VCPKG_ROOT)
  message(FATAL_ERROR "VCPKG_ROOT is not set. Please define VCPKG_ROOT as an environment variable.")
endif()

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
PROJECT(GraphMining)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
INCLUDE_DIRECTORIES(include)
#SET(CMAKE_C_COMPILER "/opt/gcc-7.3.0/bin/gcc")
# SET(CMAKE_BUILD_TYPE DEBUG)
SET(CMAKE_BUILD_TYPE RELEASE)
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -fsanitize=address")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -fsanitize=address")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# 设置要使用的 g++ 版本
set(CMAKE_CXX_COMPILER "/usr/bin/g++-9")

# 设置要使用的 gcc 版本
set(CMAKE_C_COMPILER "/usr/bin/gcc-9")

# ADD_COMPILE_OPTIONS(-std=c++11)

find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    INCLUDE_DIRECTORIES(${OpenMP_INCLUDE_DIRS})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS}" )
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenMP_CXX_FLAGS}" )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}" )
    set(CMAKE_SHARE_LINKER_FLAGS "${CMAKE_SHARE_LINKER_FLAGS} ${OpenMP_SHARE_LINKER_FLAGS}" ) 
    #TARGET_COMPILE_OPTIONS(graph_mining PRIVATE -Wall ${OpenMP_CXX_FLAGS})
endif()

find_package(MPI REQUIRED)
if(MPI_FOUND)
    SET(CMAKE_CXX_COMPILER mpicxx)   
endif()

ADD_SUBDIRECTORY(src)
#ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(tianhe)

MESSAGE("CMAKE_C_FLAGS = " ${CMAKE_C_FLAGS})
MESSAGE("CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
MESSAGE("CMAKE_CXX_FLAGS_RELEASE = " ${CMAKE_CXX_FLAGS_RELEASE})
MESSAGE("CMAKE_CXX_FLAGS_DEBUG = " ${CMAKE_CXX_FLAGS_DEBUG})
MESSAGE("CMAKE_EXE_LINKER_FLAGS" = ${CMAKE_EXE_LINKER_FLAGS})
MESSAGE("PROJECT BINARY DIR = " ${PROJECT_BINARY_DIR})
MESSAGE("INCLUDE DIRECTORIES = " ${CPLUS_INCLUDE_PATH})
MESSAGE("CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})
MESSAGE("CMAKE_INSTALL_PREFIX = " ${CMAKE_INSTALL_PREFIX})
