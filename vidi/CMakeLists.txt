cmake_minimum_required (VERSION 2.8.13)

project (vidi)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories ( ${PARSER_INC} )

if (VIDI_USE_UDIS86)
    include_directories (${MINIDIS_DIR} ${UDIS86_DIR} ${CAPSTONE_INC})
else()
    include_directories (${MINIDIS_DIR} ${CAPSTONE_INC})
endif()


set ( core_hdrs
    core/Executables.h
    core/ExeHandlerBase.h
    core/ExeHandler.h
    core/LoadProgressBar.h
    core/FileLoader.h
    core/ExeHandlerLoader.h
    core/Controller.h
    core/NavigHistory.h
)

set ( core_srcs
    core/Executables.cpp
    core/ExeHandler.cpp
    core/ExeHandlerLoader.cpp
    core/FileLoader.cpp
    core/Controller.cpp
)

set ( disasm_mv_hdrs
    disasm_mv/DisasmColorSet.h
    disasm_mv/DisasmIconSet.h
    disasm_mv/DisasmBaseModel.h
    disasm_mv/DisasmBlockModel.h
    disasm_mv/DisasmModel.h
    disasm_mv/DisasmCmpModel.h
    disasm_mv/DisasmView.h
    disasm_mv/models.h
)

set ( disasm_mv_srcs
    disasm_mv/DisasmColorSet.cpp
    disasm_mv/DisasmIconSet.cpp
    disasm_mv/DisasmBaseModel.cpp
    disasm_mv/DisasmBlockModel.cpp
    disasm_mv/DisasmModel.cpp
    disasm_mv/DisasmCmpModel.cpp
    disasm_mv/DisasmView.cpp
)

set ( lists_mv_hdrs
    lists_mv/FunctionsModel.h
    lists_mv/TagsModel.h
    lists_mv/StringsModel.h
    lists_mv/CalledImportsModel.h
    lists_mv/models.h
)

set ( lists_mv_srcs
    lists_mv/FunctionsModel.cpp
    lists_mv/TagsModel.cpp
    lists_mv/StringsModel.cpp
    lists_mv/CalledImportsModel.cpp
)

set ( srcs
    InfoTableModel.cpp
    CodeBlocksModel.cpp
    MouseTrackingTableView.cpp
    OffsetedView.cpp
    FollowableOffsetedView.cpp
    OffsetDialog.cpp
    mainwindow.cpp
    main.cpp
)

set ( frames_srcs
    frames/FilteredFrame.cpp
    frames/CodeBlocksFrame.cpp
)

set ( hdrs
    GuiUtil.h
    InfoTableModel.h
    CodeBlocksModel.h
    IndexHooverModel.h
    ReferenceSelectModel.h
    ExeDependentModel.h
    OffsetedModel.h
    MouseTrackingTableView.h
    OffsetedView.h
    FollowableOffsetedView.h
    OffsetDialog.h
    mainwindow.h
)

set ( frames_hdrs
    frames/FilteredFrame.h
    frames/CodeBlocksFrame.h
)

set ( forms 
    mainwindow.ui
    offsetdialog.ui
    frames/filteredframe.ui
    frames/codeblocksframe.ui
)

set ( rcc 
    application.qrc
)

qt5_add_resources(rcc_src ${rcc})

QT5_WRAP_UI (forms_hdrs ${forms})
QT5_WRAP_CPP (hdrs_moc ${hdrs} ${core_hdrs} ${frames_hdrs} ${disasm_mv_hdrs} ${lists_mv_hdrs} )

set ( all_hdrs
    ${core_hdrs}
    ${forms_hdrs}
    ${frames_hdrs}
    ${disasm_mv_hdrs}
    ${lists_mv_hdrs}
    ${hdrs}
)

set ( all_srcs
    ${core_srcs}
    ${frames_srcs}
    ${disasm_mv_srcs}
    ${lists_mv_srcs}
    ${srcs}
    ${rcc_src}
)

SOURCE_GROUP("Source Files\\Auto Generated" FILES ${hdrs_moc} {${rcc_src} )
SOURCE_GROUP("Source Files\\core" FILES ${core_srcs} )
SOURCE_GROUP("Header Files\\core" FILES ${core_hdrs} )
SOURCE_GROUP("Header Files\\frames" FILES ${frames_hdrs} )
SOURCE_GROUP("Source Files\\frames" FILES ${frames_srcs} )
SOURCE_GROUP("Header Files\\disasm_mv" FILES ${disasm_mv_hdrs} )
SOURCE_GROUP("Source Files\\disasm_mv" FILES ${disasm_mv_srcs} )
SOURCE_GROUP("Header Files\\lists_mv" FILES ${lists_mv_hdrs} )
SOURCE_GROUP("Source Files\\lists_mv" FILES ${lists_mv_srcs} )

IF (WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
ENDIF()

add_executable (vidi vidi.rc resource.h ${all_srcs} ${all_hdrs} ${rcc} ${hdrs_moc})

if (VIDI_USE_UDIS86)
    target_link_libraries (vidi ${MINIDIS_LIB} ${PARSER_LIB} ${UDIS86_LIB} ${CAPSTONE_LIB} Qt5::Core Qt5::Widgets )
    MESSAGE( "Compiling using udis86" )
else()
    target_link_libraries (vidi ${MINIDIS_LIB} ${PARSER_LIB} ${CAPSTONE_LIB} Qt5::Core Qt5::Widgets)
    MESSAGE( "Compiling using capstone" )
endif()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

