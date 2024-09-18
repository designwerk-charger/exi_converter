# Module: Findpybind11.cmake
# Author: Bastian Rieck <bastian.rieck@iwr.uni-heidelberg.de>
#
# CMake find module for pybind11.

INCLUDE( FindPackageHandleStandardArgs )

SET( PYBIND11_HINT_DIR "$ENV{VIRTUAL_ENV}/lib64/python3.10/site-packages/pybind11" "$ENV{VIRTUAL_ENV}/lib/python3.11/site-packages/pybind11/include" "/usr/local/include/python3.7" "/usr/local/include/python3.8" "$ENV{STAGING_DIR}/usr/include/python3.7m/pybind11/include")

MESSAGE ("pybind hints: ${PYBIND11_HINT_DIR}")

FIND_PATH(
  PYBIND11_INCLUDE_DIR
    pybind11/pybind11.h
  HINTS
    ${PYBIND11_HINT_DIR}
)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(pybind11 DEFAULT_MSG
  PYBIND11_INCLUDE_DIR
)

IF( PYBIND11_FOUND )
  SET( PYBIND11_INCLUDE_DIRS ${PYBIND11_INCLUDE_DIR} )

  MARK_AS_ADVANCED(
    PYBIND11_INCLUDE_DIR
    PYBIND11_DIR
  )
ELSE()
  SET( PYBIND11_DIR "" CACHE STRING
    "An optional hint to a pybind11 directory"
  )
  IF ( PYBIND_FIND_REQUIRED )
    MESSAGE( FATAL_ERROR "Abbort because pybind was not found" )
  ENDIF()
ENDIF()
