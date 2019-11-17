cmake_minimum_required (VERSION 2.8.13)
project (vidi)

# modules:
set ( M_BEARPARSER "bearparser/parser" )
set ( M_MINIDIS "disasm" )
set ( M_COMMANDER "vidi" )
set ( M_CAPSTONE "capstone" )
set ( M_UDIS86 "udis86" )

# modules paths:
set (CAPSTONE_DIR "${CMAKE_SOURCE_DIR}/${M_CAPSTONE}" CACHE PATH "Capstone main path")
set (CAPSTONE_INC "${CAPSTONE_DIR}/include" CACHE PATH "Capstone include path")

option(CAPSTONE_BUILD_STATIC_RUNTIME "Embed static runtime" OFF)
option(CAPSTONE_BUILD_STATIC "Build static library" ON)
option(CAPSTONE_BUILD_SHARED "Build shared library" OFF)
option(CAPSTONE_BUILD_DIET "Build diet library" OFF)
option(CAPSTONE_BUILD_TESTS "Build tests" OFF)
option(CAPSTONE_USE_DEFAULT_ALLOC "Use default memory allocation functions" ON)

option(CAPSTONE_ARM_SUPPORT "ARM support" OFF)
option(CAPSTONE_ARM64_SUPPORT "ARM64 support" OFF)
option(CAPSTONE_MIPS_SUPPORT "MIPS support" OFF)
option(CAPSTONE_PPC_SUPPORT "PowerPC support" OFF)
option(CAPSTONE_SPARC_SUPPORT "Sparc support" OFF)
option(CAPSTONE_SYSZ_SUPPORT "SystemZ support" OFF)
option(CAPSTONE_XCORE_SUPPORT "XCore support" OFF)
option(CAPSTONE_X86_SUPPORT "x86 support" ON)
option(CAPSTONE_X86_REDUCE "x86 with reduce instruction sets to minimize library" OFF)
option(CAPSTONE_X86_ATT_DISABLE "Disable x86 AT&T syntax" ON)

option(VIDI_USE_UDIS86 "Build with udis86" OFF)

set( UDIS86_DIR "${CMAKE_SOURCE_DIR}/${M_UDIS86}" CACHE PATH "Udis86 main path")

set (PARSER_DIR "${CMAKE_SOURCE_DIR}/${M_BEARPARSER}" CACHE PATH "BearParser main path")
set (PARSER_INC "${PARSER_DIR}/include" CACHE PATH "BearParser include path")

set (MINIDIS_DIR "${CMAKE_SOURCE_DIR}/${M_MINIDIS}" CACHE PATH "MiniDis main path")
set (COMMANDER_DIR "${CMAKE_SOURCE_DIR}/${M_COMMANDER}" CACHE PATH "Tester main path")

# Add sub-directories
#
# libs
if (VIDI_USE_UDIS86)
    add_subdirectory(${M_UDIS86})
    set(UDIS86_LIB $<TARGET_FILE:libudis86> CACHE FILE "Udis86 library path")
endif ()

add_subdirectory (${M_BEARPARSER})
set (PARSER_LIB $<TARGET_FILE:bearparser> CACHE FILE "BearParser library path")

add_subdirectory (${M_MINIDIS})
set (MINIDIS_LIB $<TARGET_FILE:mini_disasm> CACHE FILE "MiniDisasm library path")

add_subdirectory (${CAPSTONE_DIR})
include_directories(${CAPSTONE_INC})
set (CAPSTONE_LIB $<TARGET_FILE:capstone-static> CACHE FILE  CapstoneLib)

# executables
add_subdirectory(vidi)

# dependencies
if (VIDI_USE_UDIS86)
    add_dependencies(vidi bearparser libudis86 mini_disasm)
else()
    add_dependencies(vidi bearparser capstone-static mini_disasm)
endif()

