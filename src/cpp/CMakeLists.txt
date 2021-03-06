#include_directories(${CMAKE_SOURCE_DIR}/include algorithms util nn .)

file(GLOB_RECURSE C_SOURCES *.cpp)
file(GLOB_RECURSE CPP_SOURCES *.cpp)

file(GLOB_RECURSE C_FLANN flann.cpp)
file(GLOB_RECURSE CPP_FLANN flann_cpp.cpp)

list(REMOVE_ITEM CPP_SOURCES ${C_FLANN})
list(REMOVE_ITEM C_SOURCES ${CPP_FLANN})


if (BUILD_C_BINDINGS)
    add_library(flann SHARED ${C_SOURCES})
    set_target_properties(flann PROPERTIES
       VERSION ${FLANN_VERSION}
       SOVERSION ${FLANN_SOVERSION}
    ) 
endif()

add_library(flann_cpp SHARED ${CPP_SOURCES})
set_target_properties(flann_cpp PROPERTIES
   VERSION ${FLANN_VERSION}
   SOVERSION ${FLANN_SOVERSION}
)


if (BUILD_C_BINDINGS)
    add_library(flann_s STATIC ${C_SOURCES})
    SET_TARGET_PROPERTIES( flann_s PROPERTIES COMPILE_FLAGS -fPIC)
endif()

add_library(flann_cpp_s STATIC ${CPP_SOURCES})
SET_TARGET_PROPERTIES( flann_cpp_s PROPERTIES COMPILE_FLAGS -fPIC)

if(WIN32)
if (BUILD_C_BINDINGS)
    install (
        TARGETS flann
        RUNTIME DESTINATION share/flann/matlab
    )
endif()
endif(WIN32)

if (BUILD_C_BINDINGS)
    install (
        TARGETS flann flann_cpp flann_s flann_cpp_s
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${FLANN_LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${FLANN_LIB_INSTALL_DIR}
    )
else()
    install (
        TARGETS flann_cpp flann_cpp_s
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${FLANN_LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${FLANN_LIB_INSTALL_DIR}
    )
endif()

install (
    DIRECTORY flann
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
