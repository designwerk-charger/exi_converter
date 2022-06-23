########################################################################
# Create the EXI Converter libray
########################################################################

########################################################################
# Set variable for generate source code
set(GENERATED_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(GENERATED_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

set(GENERATED_SOURCES)
file(GLOB_RECURSE XSD_FILES ${PROJECT_SOURCE_DIR}/schemas/*.xsd)
file(GLOB_RECURSE GENERATOR_FILES ${PROJECT_SOURCE_DIR}/code_generator/*.py)

macro(add_code_generator PROTOCOL_NAME SCHEMA_FILE)

    set(GENERATED_SOURCES_${PROTOCOL_NAME}
            ${GENERATED_FILES_DIR}/${PROTOCOL_NAME}/enum_types.cpp
            ${GENERATED_FILES_DIR}/${PROTOCOL_NAME}/complex_types.cpp
            ${GENERATED_FILES_DIR}/${PROTOCOL_NAME}/body_message.cpp
    )

    set(GENERATED_HEADERS_${PROTOCOL_NAME}
            ${GENERATED_FILES_DIR}/${PROTOCOL_NAME}/enum_types.h
            ${GENERATED_FILES_DIR}/${PROTOCOL_NAME}/complex_types.h
            ${GENERATED_FILES_DIR}/${PROTOCOL_NAME}/body_message.h
    )

    add_custom_command(
            OUTPUT ${GENERATED_SOURCES_${PROTOCOL_NAME}} ${GENERATED_HEADERS_${PROTOCOL_NAME}}
            COMMAND python3 ARGS ${PROJECT_SOURCE_DIR}/code_generator/main.py --namespace=${PROTOCOL_NAME} --output_path=${GENERATED_FILES_DIR}/${PROTOCOL_NAME}/ --schema_file=${PROJECT_SOURCE_DIR}/schemas/${SCHEMA_FILE}
            DEPENDS ${XSD_FILES} ${GENERATOR_FILES}
    )

    add_custom_target (
            generated_files_for_${PROTOCOL_NAME} DEPENDS
            ${GENERATED_SOURCES_${PROTOCOL_NAME}}
            ${GENERATED_HEADERS_${PROTOCOL_NAME}}
    )

    set_property(SOURCE ${GENERATED_SOURCES_${PROTOCOL_NAME}} PROPERTY GENERATED TRUE)

    set(GENERATED_SOURCES ${GENERATED_SOURCES} ${GENERATED_SOURCES_${PROTOCOL_NAME}})
endmacro()


add_code_generator(iso15118_2 iso15118_2/V2G_CI_MsgDef.xsd)
add_code_generator(app_protocol V2G_CI_AppProtocol.xsd)

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
add_dependencies(exi_converter
        generated_files_for_app_protocol
        generated_files_for_iso15118_2)

# Remove the "lib" prefix of the .so library
set_target_properties(exi_converter PROPERTIES PREFIX "")

# Add the __init__.py in next to the library
configure_file(__init__.py ${CMAKE_CURRENT_BINARY_DIR}/__init__.py)

set(LINT_SRCS ${LINT_SRCS} ${LIB_SRCS} PARENT_SCOPE)
