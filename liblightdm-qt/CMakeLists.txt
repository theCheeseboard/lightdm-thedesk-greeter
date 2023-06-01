project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIGHTDM_GOBJECT REQUIRED IMPORTED_TARGET liblightdm-gobject-1)
pkg_check_modules(GOBJECT REQUIRED IMPORTED_TARGET gobject-2.0)

set(SOURCES
        greeter.cpp
        power.cpp
        sessionsmodel.cpp
        usersmodel.cpp
    )

set(HEADERS
        QLightDM/Greeter
        QLightDM/Power
        QLightDM/SessionsModel
        QLightDM/UsersModel
        QLightDM/greeter.h
        QLightDM/power.h
        QLightDM/sessionsmodel.h
        QLightDM/usersmodel.h
    )

add_library(liblightdm STATIC ${SOURCES} ${HEADERS})
set_target_properties(liblightdm PROPERTIES
        OUTPUT_NAME lightdm-qt
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.vicr123.liblightdm
        VERSION 1.0.0
        PUBLIC_HEADER "${HEADERS}"
        AUTOMOC ON
    )

target_link_libraries(liblightdm Qt::Widgets PkgConfig::LIGHTDM_GOBJECT PkgConfig::GOBJECT)
target_compile_definitions(liblightdm PRIVATE
    liblightdm_LIBRARY
    QT_NO_KEYWORDS
)
