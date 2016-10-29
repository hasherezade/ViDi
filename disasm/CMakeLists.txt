cmake_minimum_required(VERSION 2.8)
project (mini_disasm)

message (STATUS "parser_dir='${PARSER_DIR}'")
message (STATUS "parser_lib='${PARSER_LIB}'")
message(STATUS "udis_dir='${UDIS86_DIR}'")
message(STATUS "udis_lib='${UDIS86_LIB}'")
message(STATUS "capstone_dir='${CAPSTONE_DIR}'")
message(STATUS "capstone_lib='${CAPSTONE_LIB}'")

find_package (Qt4 REQUIRED)

include_directories (${PARSER_DIR} ${UDIS86_DIR} ${CAPSTONE_INC} ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR})

set (hdrs
    AddrConverter.h
    MnemType.h
    Context.h
    TargetValue.h
    DisasmChunk.h
    DisasmChunkBuf.h
    CodeBlock.h
    FuncNameManager.h
    DisasmBase.h
    Tracer.h
    ExeDisasm.h
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
)

set (cd_srcs
    cdis/CapstoneChunk.cpp
    cdis/CDisasm.cpp
)

set (pe_hdrs
    pe/PeDataFetcher.h
    pe/PeDisasm.h
    pe/PeTracer.h
)

set (pe_srcs
    pe/PeDisasm.cpp
    pe/PeTracer.cpp
)

set (dos_hdrs
    dos/DosDisasm.h
    dos/DosTracer.h
)

set (dos_srcs
    dos/DosDisasm.cpp
    dos/DosTracer.cpp
)

QT4_WRAP_CPP (hdrs_moc ${hdrs} ${pe_hdrs} ${dllparse_hdrs})

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

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

add_library (mini_disasm STATIC ${hdrs_moc} ${hdrs} ${srcs} ${pe_hdrs} ${pe_srcs} ${dos_hdrs} ${dos_srcs} ${ud_hdrs} ${ud_srcs} ${cd_hdrs} ${cd_srcs})
if (VIDI_USE_UDIS86)
    target_link_libraries (mini_disasm ${PARSER_LIB} ${UDIS86_LIB} ${CAPSTONE_LIB} ${QT_QTGUI_LIBRARIES} ${QT_QTCORE_LIBRARIES})
else()
    target_link_libraries (mini_disasm ${PARSER_LIB} ${CAPSTONE_LIB} ${QT_QTGUI_LIBRARIES} ${QT_QTCORE_LIBRARIES})
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
    add_dependencies(mini_disasm bearparser libudis86 capstone-static)
else()
    add_dependencies(mini_disasm bearparser capstone-static)
endif()
