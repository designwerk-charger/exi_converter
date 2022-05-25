########################################################################
# Adding exi converter tests
########################################################################
cmake_minimum_required(VERSION 3.8.1)

# Collect test sources and include directories
file(GLOB TEST_TYPE_CONV_SRC type_conversion/*.cpp)
file(GLOB TEST_BASE_SRC base/*.cpp)
file(GLOB TEST_INTEGRATION_SRC integration_test/*.cpp)
set(TEST_SRCS ${TEST_TYPE_CONV_SRC} ${TEST_BASE_SRC} ${TEST_INTEGRATION_SRC})

include_directories(../testlib/googletest-release-1.10.0/googletest/include)
include_directories(../testlib/googletest-release-1.10.0/googlemock/include)
include_directories(../lib)
include_directories(.)
include_directories(${GENERATED_FILES_DIR})


add_definitions(-DPROJECT_ROOT=\"${CMAKE_SOURCE_DIR}\")


# Configure the test executable
add_executable(exi_converter_test ${TEST_SRCS} tests.cpp)
add_dependencies(exi_converter_test exi_converter gtest gmock)
target_link_libraries(exi_converter_test PUBLIC gtest gmock exi_converter ${PYTHON_LIBRARY})


# Configure GoogleTest framework
include(GoogleTest)
gtest_add_tests(TARGET exi_converter_test
                TEST_LIST   all_exi_converter_test)
set_tests_properties(${all_exi_converter_test} PROPERTIES TIMEOUT 10)

set(LINT_SRCS ${LINT_SRCS} ${TEST_SRCS} PARENT_SCOPE)