project(greeter VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(libcontemporary REQUIRED)
find_package(libtdesktopenvironment REQUIRED)

set(SOURCES
    actionqueue.cpp
    differentuserpopover.cpp
    main.cpp
    mainwindow.cpp
    operatingsystemselect.cpp
    poweroptions.cpp
    sessionmenu.cpp
    userlistpane.cpp

    differentuserpopover.ui
    mainwindow.ui
    operatingsystemselect.ui
    poweroptions.ui
    userlistpane.ui
)

set(HEADERS
    actionqueue.h
    differentuserpopover.h
    mainwindow.h
    operatingsystemselect.h
    poweroptions.h
    sessionmenu.h
    userlistpane.h
)

add_executable(greeter ${SOURCES} ${HEADERS})

set_target_properties(greeter PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.greeter
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR greeter)

cntp_init(greeter 20)
cntp_translate(greeter)
cntp_target_name(greeter "lightdm-thedesk-greeter")
cntp_defaults_file(greeter
    DEFAULTS_FILE lightdm-thedesk-greeter.conf)

target_link_libraries(greeter Qt6::Widgets libcontemporary liblightdm libtdesktopenvironment)
target_include_directories(greeter PRIVATE ../liblightdm-qt)

install(TARGETS greeter
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION /Applications
)

install(FILES lightdm-thedesk-greeter.desktop
    DESTINATION /usr/share/xgreeters
)
