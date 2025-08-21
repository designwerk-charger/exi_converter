########################################################################
# CMake for EXI Converter Library with bindings for Python
########################################################################
cmake_minimum_required(VERSION 3.10)

project(exi_converter CXX)

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
include( CheckCXXCompilerFlag )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

list( APPEND CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/cmake
  ${CMAKE_SOURCE_DIR}/cmake/Modules
)


########################################################################
# Enable compiler feature to generate a coverage report
#    This is done with cmake ... -DUSE_GCOV=ON or automatically with When CLion (Jetbrains) is used
option(USE_GCOV "Create a GCov-enabled build." OFF)

if ($ENV{JETBRAINS_IDE})
    set(USE_GCOV "ON")
endif()

if (${USE_GCOV})
    MESSAGE("Build with GCov-enabled!")
    set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
    set(GCC_COVERAGE_LINK_FLAGS    "-lgcov")
endif()
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )


########################################################################
# Find the necessary dependencies
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

include_directories(${Python3_INCLUDE_DIRS} ${PYBIND11_INCLUDE_DIRS})


########################################################################
# Adding the Library specified in the next cmake file
add_subdirectory(lib/)
add_subdirectory(testlib/)
add_subdirectory(test/)
include_directories(lib/)

file(GLOB LIB_SRCS )
set(LIB_SRCS lib/library.cpp lib/base/bitstream.cpp lib/type_conversion/base_types.cpp)


set(_EXE_SRC converter_tool/main.cpp)
add_executable(converter_tool ${_EXE_SRC})
add_dependencies(converter_tool exi_converter)
target_link_libraries(converter_tool PUBLIC exi_converter ${Python3_LIBRARIES})

########################################################################
# style checks
setup_cpplint(exi_converter ${LINT_SRCS})
