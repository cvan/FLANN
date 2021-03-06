cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(flann)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

include(${PROJECT_SOURCE_DIR}/cmake/flann_utils.cmake)
set(FLANN_VERSION 1.6.4)
DISSECT_VERSION()
GET_OS_INFO()

# Add an "uninstall" target
CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/cmake/uninstall_target.cmake.in"
    "${PROJECT_BINARY_DIR}/uninstall_target.cmake" IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET (uninstall "${CMAKE_COMMAND}" -P
    "${PROJECT_BINARY_DIR}/uninstall_target.cmake")

if (NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX /usr/local)
endif()

# Set the build type.  Options are:
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries

if (NOT DEFINED CMAKE_BUILD_TYPE)
    #set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
    #set(CMAKE_BUILD_TYPE Debug)
endif()

message("Install prefix: ${CMAKE_INSTALL_PREFIX}")
message("Build type: ${CMAKE_BUILD_TYPE}")


#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# set output path for tests
set(TEST_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/test)


if (NOT DEFINED BUILD_C_BINDINGS)
    set(BUILD_C_BINDINGS true)
endif()

if (NOT DEFINED BUILD_PYTHON_BINDINGS)
    set(BUILD_PYTHON_BINDINGS true)
endif()

if (NOT DEFINED BUILD_MATLAB_BINDINGS)
    set(BUILD_MATLAB_BINDINGS true)
endif()

if (NOT BUILD_C_BINDINGS)
    set(BUILD_PYTHON_BINDINGS false)
    set(BUILD_MATLAB_BINDINGS false)
endif()

message("Building C bindings: ${BUILD_C_BINDINGS}")
message("Building python bindings: ${BUILD_PYTHON_BINDINGS}")
message("Building matlab bindings: ${BUILD_MATLAB_BINDINGS}")


message("USE_MPI: ${USE_MPI}")

find_hdf5()
if (USE_MPI OR HDF5_IS_PARALLEL)
    find_package(MPI)
endif()

if (HDF5_IS_PARALLEL)
    if (NOT MPI_FOUND)
        message(ERROR "Found the parallel HDF5 library, but could not find the MPI library. Define the MPI_COMPILER variable to the path of your MPI compiler.")
    endif()
    # Parallel HDF5 needs to find the "mpi.h" header file
    include_directories(${MPI_INCLUDE_PATH})
endif()


if (USE_MPI)
    if (NOT MPI_FOUND)
        message(ERROR "Could not find an MPI library. Define the MPI_COMPILER variable to the path of your MPI compiler.")
    endif()

    if (NOT HDF5_IS_PARALLEL)
        message(ERROR "For MPI support the Parallel HDF5 library is required.")
    endif()

endif(USE_MPI)



#set the C/C++ include path to the "include" directory
include_directories(${PROJECT_SOURCE_DIR}/src/cpp)

# require proper c++
#add_definitions( "-Wall -ansi -pedantic" )
# HDF5 uses long long which is not ansi
add_definitions( "-Wall" )

add_subdirectory( cmake )
add_subdirectory( src )
add_subdirectory( examples )
add_subdirectory( test )
add_subdirectory( doc )
