cmake_minimum_required(VERSION 2.8)
project (mini_disasm)

message (STATUS "parser_dir='${PARSER_DIR}'")
message (STATUS "parser_lib='${PARSER_LIB}'")

message(STATUS "udis_dir='${UDIS86_DIR}'")
message(STATUS "udis_lib='${UDIS86_LIB}'")

message(STATUS "capstone_includes='${CAPSTONE_INC}'")
message(STATUS "capstone_lib='${CAPSTONE_LIB}'")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories (${PARSER_INC} ${UDIS86_DIR} ${CAPSTONE_INC})

include_directories (
  include
)

set (hdrs
    include/AddrConverter.h
    include/MnemType.h
    include/Context.h
    include/TargetValue.h
    include/DisasmChunk.h
    include/DisasmChunkBuf.h
    include/CodeBlock.h
    include/FuncNameManager.h
    include/DisasmBase.h
    include/Tracer.h
    ExeDisasm.h
    Util.h
)

if (VIDI_USE_UDIS86)
    set (ud_hdrs
        udis/UdisChunk.h
        udis/UDisasm.h
        udis/ExeDisasm.h
    )
    set (ud_srcs
        udis/UdisChunk.cpp
        udis/UDisasm.cpp
    )
endif ()


set (cd_hdrs
    cdis/CapstoneChunk.h
    cdis/CDisasm.h
    cdis/ExeDisasm.h
)

set (srcs
    Context.cpp
    CodeBlock.cpp
    FuncNameManager.cpp
    DisasmBase.cpp
    DisasmChunk.cpp
    DisasmChunkBuf.cpp
    Tracer.cpp
    Util.cpp
)

set (cd_srcs
    cdis/CapstoneChunk.cpp
    cdis/CDisasm.cpp
)

set (pe_hdrs
    include/pe/PeDataFetcher.h
    include/pe/PeDisasm.h
    include/pe/PeTracer.h
)

set (pe_srcs
    pe/PeDisasm.cpp
    pe/PeTracer.cpp
)

set (dos_hdrs
    include/dos/DosDisasm.h
    include/dos/DosTracer.h
)

set (dos_srcs
    dos/DosDisasm.cpp
    dos/DosTracer.cpp
)

QT5_WRAP_CPP (hdrs_moc ${hdrs} ${pe_hdrs} ${dllparse_hdrs})

SOURCE_GROUP("Source Files\\Auto Generated" FILES ${hdrs_moc} ${rcc_src} )

SOURCE_GROUP("Source Files\\pe" FILES ${pe_srcs} )
SOURCE_GROUP("Header Files\\pe" FILES ${pe_hdrs} )

SOURCE_GROUP("Source Files\\dos" FILES ${dos_srcs} )
SOURCE_GROUP("Header Files\\dos" FILES ${dos_hdrs} )

SOURCE_GROUP("Source Files\\cdis" FILES ${cd_srcs} )
SOURCE_GROUP("Header Files\\cdis" FILES ${cd_hdrs} )

if (VIDI_USE_UDIS86)
    SOURCE_GROUP("Source Files\\udis" FILES ${ud_srcs} )
    SOURCE_GROUP("Header Files\\udis" FILES ${ud_hdrs} )
endif ()

add_library (mini_disasm STATIC ${hdrs_moc} ${hdrs} ${srcs} ${pe_hdrs} ${pe_srcs} ${dos_hdrs} ${dos_srcs} ${ud_hdrs} ${ud_srcs} ${cd_hdrs} ${cd_srcs})
if (VIDI_USE_UDIS86)
    target_link_libraries (mini_disasm ${PARSER_LIB} ${UDIS86_LIB} Qt5::Core Qt5::Widgets )
else()
    target_link_libraries (mini_disasm ${PARSER_LIB} ${CAPSTONE_LIB} Qt5::Core Qt5::Widgets)
endif()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# defines setting:
if (VIDI_USE_UDIS86)
    set (def
        "BUILD_WITH_UDIS86"
        "USE_UDIS86"
    )
    list(APPEND deflist ${def})
    set_target_properties(mini_disasm PROPERTIES COMPILE_DEFINITIONS "${deflist}")
endif()

# dependencies
if (VIDI_USE_UDIS86)
    add_dependencies(mini_disasm bearparser libudis86 )
else()
    add_dependencies(mini_disasm bearparser capstone-static)
endif()

