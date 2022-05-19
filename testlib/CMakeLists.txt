########################################################################
# Add google c++ test framework
########################################################################

set(INSTALL_GTEST OFF)
add_subdirectory(googletest-release-1.10.0)
include(cpplint/cpplint.cmake)


