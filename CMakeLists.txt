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
    src/close.c
    src/codepage.c
    src/crt0.s
    src/getres.c
    src/gettime.c
    src/gettimezone.c
    src/gettimespec.c
    src/initenv.s
    src/irq.s
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
    src/settime.c
    src/stdin_opt.c
    src/sysremove.c
    src/sysrename.c
    src/write_xram.c
    src/write_xstack.c
    src/write.c
    src/xreg.s
)

# Package a CC65 executable target as an RP6502 ROM.
#
# RP6502 Executables
# ^^^^^^^^^^^^^^^^^^
#
#  rp6502_executable(<name> START [addr] RESET [addr] roms...)
#
# Packages an executable target called ``<name>`` in RP6502 ROM format.
# ``START <addr>`` defaults to 0x200.
# ``RESET <addr>`` defaults to 0x200.
#
function(rp6502_executable name)
    # Parse args
    set(start_addr "0x200")
    set(reset_addr "0x200")
    set(extra_roms)
    foreach(X IN LISTS ARGN)
        if (NOT start_addr)
            set(start_addr ${X})
        elseif (NOT reset_addr)
            set(reset_addr ${X})
        elseif (X STREQUAL "START")
            set(start_addr FALSE)
        elseif (X STREQUAL "RESET")
            set(reset_addr FALSE)
        else ()
            list(APPEND extra_roms ${X})
        endif()
    endforeach()
    if (NOT start_addr)
        message (FATAL_ERROR "rp6502_executable START address missing")
    elseif (NOT reset_addr)
        message (FATAL_ERROR "rp6502_executable RESET address missing")
    endif()
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
            -- ${extra_roms}
        DEPENDS ${name})
endfunction()

# Package anything as an RP6502 ROM.
function(rp6502_asset name addr in_file)
    # Parse optional args
    get_filename_component(out_file ${in_file} NAME)
    set(out_file "${out_file}.rp6502")
    set(custom_target_name "${name}.${addr}.${out_file}")
    set(extra_args ${ARGN})
    list(LENGTH extra_args extra_count)
    if (${extra_count} GREATER 0)
        list(GET ${ARGN} 0 out_file)
    endif ()
    add_custom_target(
        ${custom_target_name} ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${out_file}
    )
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${out_file}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${in_file}
        COMMAND
            "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/rp6502-sdk/rp6502.py"
            -a "${addr}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/${out_file}"
            create "${CMAKE_CURRENT_SOURCE_DIR}/${in_file}"
    )
endfunction()
