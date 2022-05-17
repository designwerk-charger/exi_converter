# Copyright (C) 2013 Daniel Scharrer
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the author(s) be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

# Copyright (C) 2014 Greg Horn
# In order to comply with the above copyright, I am noting that I took
# Daniel's script and hacked it a bit, mostly changing paths and filters

# Copyright (C) 2015 Supercomputing Systems AG
# Performance optimized by Christian Lang

option(WITH_CPPLINT "Run cpplint before compiling" ON)
if(WITH_CPPLINT)

find_package(PythonInterp)
if (NOT PYTHONINTERP_FOUND)
    message(FATAL_ERROR "python not found")
endif ()
set(CPPLINT_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)


################################################################
# Setup cpplint check target and let the provided target depend
# on it.
#
# TARGET The name of the target which will depend on cpplint checks.
# ARGN List of all source files to check.
function(setup_cpplint TARGET)
    set(SOURCES_LIST ${ARGN})
    if (SOURCES_LIST)
        set(CPPLINT_TARGET ${TARGET}-cpplint)
        add_style_check_target(${CPPLINT_TARGET} ${SOURCES_LIST})
        add_dependencies(${TARGET} ${CPPLINT_TARGET})
    endif ()
endfunction()
################################################################


################################################################
# uncomment to disable unwanted filters
# (in addition ensure that the filter parameter in the below COMMAND is not commented out)
set(STYLE_FILTER )
set(STYLE_FILTER ${STYLE_FILTER}-legal/copyright,)
#set(STYLE_FILTER ${STYLE_FILTER}-build/header_guard,)
set(STYLE_FILTER ${STYLE_FILTER}-build/c++11,)
#set(STYLE_FILTER ${STYLE_FILTER}-build/c++14,)
#
#set(STYLE_FILTER ${STYLE_FILTER}-whitespace/braces,)
#set(STYLE_FILTER ${STYLE_FILTER}-whitespace/semicolon,)
#set(STYLE_FILTER ${STYLE_FILTER}-whitespace/blank_line,)
#set(STYLE_FILTER ${STYLE_FILTER}-whitespace/comma,)
#set(STYLE_FILTER ${STYLE_FILTER}-whitespace/operators,)
#set(STYLE_FILTER ${STYLE_FILTER}-whitespace/parens,)
#set(STYLE_FILTER ${STYLE_FILTER}-whitespace/indent,)
#set(STYLE_FILTER ${STYLE_FILTER}-whitespace/comments,)
#set(STYLE_FILTER ${STYLE_FILTER}-whitespace/newline,)
#set(STYLE_FILTER ${STYLE_FILTER}-whitespace/tab,)
#
#set(STYLE_FILTER ${STYLE_FILTER}-build/include,)
#set(STYLE_FILTER ${STYLE_FILTER}-build/include_order,)
#set(STYLE_FILTER ${STYLE_FILTER}-build/namespaces,)
#set(STYLE_FILTER ${STYLE_FILTER}-build/include_what_you_use,)
#
#set(STYLE_FILTER ${STYLE_FILTER}-readability/streams,)
#set(STYLE_FILTER ${STYLE_FILTER}-readability/todo,)
#
#set(STYLE_FILTER ${STYLE_FILTER}-runtime/references,)
#set(STYLE_FILTER ${STYLE_FILTER}-runtime/int,)
#set(STYLE_FILTER ${STYLE_FILTER}-runtime/explicit,)
set(STYLE_FILTER ${STYLE_FILTER}-runtime/printf,)
set(STYLE_FILTER ${STYLE_FILTER}-build/include_subdir,)
set(STYLE_FILTER ${STYLE_FILTER} PARENT_SCOPE)
message("current style check filter is active: ${STYLE_FILTER}")
################################################################


################################################################
# Add a target that runs cpplint.py. Only new files and files
# that still have errors are checked.
#
# Parameters:
# - TARGET_NAME The name of the target to add.
# - SOURCES_LIST A complete list of source and include files to check.
function(add_style_check_target TARGET_NAME SOURCES_LIST)
    set(SOURCES_LIST ${SOURCES_LIST} ${ARGN})

    # prepare source list
    list(REMOVE_DUPLICATES SOURCES_LIST)
    list(SORT SOURCES_LIST)

    # define output dir for timestamp files
    set(CPPLINT_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/cpplint")

    # iterate over all to check source files
    set(CPPLINT_SOURCES)
    foreach (SOURCE ${SOURCES_LIST})
        # prepare absolute path for timestamp files in cpplint build folder
        file(RELATIVE_PATH DEST ${CMAKE_CURRENT_SOURCE_DIR} ${SOURCE})
        set(DEST "${CPPLINT_BUILD_DIR}/${DEST}.timestamp")

        # define rule to make timestamp files
        add_style_check_rule(${SOURCE} ${DEST})

        # append this file to list
        set(CPPLINT_SOURCES ${CPPLINT_SOURCES} ${DEST})
    endforeach ()

    # define cpplint target that depends on all defined timestamp files
    add_custom_target(${TARGET_NAME} DEPENDS ${CPPLINT_SOURCES})
endfunction()
################################################################


################################################################
# Add a custom rule that checks a source file with cpplint.py if
# no timestamp file for that source is available.
# The timestamp file is only generated if no errors are
# found. Because we depend the rule on the timestamp file, the
# checking is only done if the file has changed or
# no timestamp is available (if previous check found errors).
#
# Parameters:
# - SOURCE The name of the to check source file.
# - DEST The timestamp file that will be generated if no errors are found.
function(add_style_check_rule SOURCE DEST)
    # prepare subdirectory in cpplint folder
    get_filename_component(CPPLINT_SUB_FOLDER ${DEST} PATH)
    file(MAKE_DIRECTORY ${CPPLINT_SUB_FOLDER})

    # add custom rule to make cpplint timestamp file
    add_custom_command(
            OUTPUT "${DEST}"
            COMMAND ${CMAKE_COMMAND} -E chdir
            ${CMAKE_SOURCE_DIR}
            ${PYTHON_EXECUTABLE}
            ${CPPLINT_CMAKE_DIR}/cpplint.py
            --filter=${STYLE_FILTER}
            --counting=toplevel
            --extensions=cc,cpp,cxx,h,hpp,hxx
            --headers=h,hpp,hxx
            --linelength=120
            --quiet
            ${SOURCE}
            && touch ${DEST}
            DEPENDS "${SOURCE}"
            COMMENT "Linting with cpplint: ${SOURCE}")
endfunction()
################################################################

else()  # if(WITH_CPPLINT)
################################################################
# Dummy implementation to allow disabling the functionality of
# this file.
function(setup_cpplint)
endfunction()
################################################################
endif()
