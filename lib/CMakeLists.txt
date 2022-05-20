########################################################################
# Create the EXI Converter libray
########################################################################

########################################################################
# Set variable for generate source code
set(GENERATED_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(GENERATED_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

set(GENERATED_SOURCES
        ${GENERATED_FILES_DIR}/enum_types.cpp
)

set(GENERATED_HEADERS
        ${GENERATED_FILES_DIR}/enum_types.h
        ${GENERATED_FILES_DIR}/base_types.h
)

########################################################################
# Generate source code for xml schemas
file(GLOB_RECURSE XSD_FILES ${PROJECT_SOURCE_DIR}/schemas/*.xsd)
file(GLOB_RECURSE GENERATOR_FILES ${PROJECT_SOURCE_DIR}/code_generator/*.py)
add_custom_command(
        OUTPUT ${GENERATED_SOURCES} ${GENERATED_HEADERS}
        COMMAND python3 ARGS ${PROJECT_SOURCE_DIR}/code_generator/main.py --output_path=${GENERATED_FILES_DIR} --schema_path=${PROJECT_SOURCE_DIR}/schemas
        DEPENDS ${XSD_FILES} ${GENERATOR_FILES}
)


add_custom_target (
        exi_converter_generated_files DEPENDS
        ${GENERATED_SOURCES}
        ${GENERATED_HEADERS}
)

set_property(SOURCE ${GENERATED_SOURCES} PROPERTY GENERATED TRUE)



########################################################################
# Add souces and configure include
file(GLOB_RECURSE _BASE_SOURCES base/*.cpp)
file(GLOB_RECURSE _TC_SOURCES type_conversion/*.cpp)


set(LIB_SRCS ${_BASE_SOURCES} ${_TC_SOURCES})

include_directories(.)
include_directories(${GENERATED_FILES_DIR})

########################################################################
# Configure the library
add_definitions(-DPYBIND)

add_library(exi_converter SHARED ${LIB_SRCS} ${GENERATED_SOURCES} library.cpp)
add_dependencies(exi_converter exi_converter_generated_files)

# Remove the "lib" prefix of the .so library
set_target_properties(exi_converter PROPERTIES PREFIX "")

# Add the __init__.py in next to the library
configure_file(__init__.py ${CMAKE_CURRENT_BINARY_DIR}/__init__.py)

set(LINT_SRCS ${LINT_SRCS} ${LIB_SRCS} PARENT_SCOPE)
