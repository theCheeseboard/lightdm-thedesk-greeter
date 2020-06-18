QT       += core gui thelib tdesktopenvironment
SHARE_APP_NAME = lightdm-thedesk-greeter

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Include the-libs build tools
include(/usr/share/the-libs/pri/buildmaster.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    actionqueue.cpp \
    differentuserpopover.cpp \
    main.cpp \
    mainwindow.cpp \
    pamchallengepopover.cpp \
    passwordpane.cpp \
    poweroptions.cpp \
    readypane.cpp \
    sessionmenu.cpp \
    userlistpane.cpp

HEADERS += \
    actionqueue.h \
    differentuserpopover.h \
    mainwindow.h \
    pamchallengepopover.h \
    passwordpane.h \
    poweroptions.h \
    readypane.h \
    sessionmenu.h \
    userlistpane.h

FORMS += \
    differentuserpopover.ui \
    mainwindow.ui \
    pamchallengepopover.ui \
    passwordpane.ui \
    poweroptions.ui \
    readypane.ui \
    userlistpane.ui

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += liblightdm-qt5-3

    target.path = /usr/bin

    desktop.files = lightdm-thedesk-greeter.desktop
    desktop.path = /usr/share/xgreeters

    INSTALLS += target desktop
}


DISTFILES += \
    lightdm-thedesk-greeter.desktop
