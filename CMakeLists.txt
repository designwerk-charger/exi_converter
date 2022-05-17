cmake_minimum_required(VERSION 3.19)
project(exi_converter)

set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
include( CheckCXXCompilerFlag )

list( APPEND CMAKE_MODULE_PATH
        ${CMAKE_SOURCE_DIR}/cmake
        ${CMAKE_SOURCE_DIR}/cmake/Modules
        )

########################################################################
# Find the necessary dependencies
find_package(PythonInterp 3 REQUIRED)
find_package(PythonLibs   3 REQUIRED)
find_package( pybind11 REQUIRED )

include_directories(${PYTHON_INCLUDE_DIRS} ${PYBIND11_INCLUDE_DIRS})


add_subdirectory(testlib/)
add_subdirectory(test/)
include_directories(lib/)

file(GLOB LIB_SRCS )
set(LIB_SRCS lib/library.cpp lib/base/bitstream.cpp lib/type_conversion/base_types.cpp)

add_definitions(-DPYBIND)
add_library(exi_converter SHARED ${LIB_SRCS})
# Remove the "lib" prefix of the .so library
set_target_properties(exi_converter PROPERTIES PREFIX "")

set(_EXE_SRC converter_tool/main.cpp)
add_executable(converter_tool ${_EXE_SRC})
add_dependencies(converter_tool exi_converter)
target_link_libraries(converter_tool PUBLIC exi_converter ${PYTHON_LIBRARY})
