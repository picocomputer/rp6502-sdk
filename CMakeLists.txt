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
    src/crt0.s
    src/rp6502.c
    src/close.c
    src/initenv.s
    src/lseek.c
    src/mainargs.s
    src/open.c
    src/oserrlist.s
    src/oserror.s
    src/read.c
    src/write.c
    src/randomize.c
    src/rtc.c
)
