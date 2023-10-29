#
# Copyright (c) 2023 Rumbledethumps
#
# SPDX-License-Identifier: BSD-3-Clause
# SPDX-License-Identifier: Unlicense
#

if(DEFINED ENV{CC65_HOME})
    set(CC65_INCLUDE "$ENV{CC65_HOME}/include")
else()
    set(CC65_INCLUDE "/usr/share/cc65/include")
endif()

add_library(rp6502 STATIC EXCLUDE_FROM_ALL)
target_include_directories(rp6502 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CC65_INCLUDE}
)
target_sources(rp6502 PUBLIC
    src/clock_getres.c
    src/clock_gettime.c
    src/clock_gettimezone.c
    src/clock_gettimespec.c
    src/clock_settime.c
    src/close.c
    src/codepage.c
    src/crt0.s
    src/initenv.s
    src/lrand.c
    src/lseek.c
    src/mainargs.s
    src/open.c
    src/oserrlist.s
    src/oserror.s
    src/phi2.c
    src/randomize.c
    src/read_xram.c
    src/read_xstack.c
    src/read.c
    src/ria.s
    src/sysremove.c
    src/sysrename.c
    src/write_xram.c
    src/write_xstack.c
    src/write.c
    src/xreg.s
)

# Package an executable target as an RP6502 ROM.
#
# RP6502 Executables
# ^^^^^^^^^^^^^^^^^^
#
#  rp6502_executable(<name> [start_addr] [reset_addr])
#
# Packages an executable target called ``<name>`` in RP6502 ROM format.
# ``<start_addr>`` defaults to 0x200.
# ``<reset_addr>`` defaults to ``<start_addr>``.
#
function(rp6502_executable name)
    # Parse optional args
    set(start_addr "0x200")
    set(reset_addr ${start_addr})
    set(extra_args ${ARGN})
    list(LENGTH extra_args extra_count)
    if (${extra_count} GREATER 0)
        list(GET extra_args 0 optional_arg)
        set(start_addr ${optional_arg})
        set(reset_addr ${start_addr})
    endif ()
    if (${extra_count} GREATER 1)
        list(GET extra_args 1 optional_arg)
        set(reset_addr ${optional_arg})
    endif ()
    list(SUBLIST extra_args 2 -1 extra_args)
    # Remove old ROM
    add_custom_command(TARGET ${name} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_CURRENT_BINARY_DIR}/${name}.rp6502"
    )
    # Create new ROM
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/rp6502-sdk/rp6502.py"
            -a "${start_addr}" -r "${reset_addr}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/${name}.rp6502"
            create "${CMAKE_CURRENT_BINARY_DIR}/${name}"
            ${extra_args}
        DEPENDS ${name})
endfunction()

# Package anything as an RP6502 ROM.
function(rp6502_asset name addr in_file)
    # Parse optional args
    get_filename_component(out_file ${in_file} NAME_WLE)
    set(extra_args ${ARGN})
    list(LENGTH extra_args extra_count)
    if (${extra_count} GREATER 0)
        list(GET ${ARGN} 0 optional_arg)
        get_filename_component(out_file ${optional_arg} NAME_WLE)
    endif ()
    add_custom_target(
        Name ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${out_file}.rp6502
    )
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${out_file}.rp6502
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${in_file}
        COMMAND
            "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/rp6502-sdk/rp6502.py"
            -a "${addr}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/${out_file}.rp6502"
            create "${CMAKE_CURRENT_SOURCE_DIR}/${in_file}"
    )
endfunction()
