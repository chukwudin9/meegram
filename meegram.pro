INCLUDEPATH       += src/
TARGET             = meegram
TEMPLATE           = app

# PRECOMPILED_HEADER = src/PrecompiledHeader.hpp
# CONFIG            += precompile_header

QT += dbus
DBUS_ADAPTORS += src/authentication/Authorization.xml

folder_01.source = resources/qml
folder_01.target = .
DEPLOYMENTFOLDERS = folder_01

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable

CONFIG += mobility
MOBILITY += systeminfo

# Submodules
include(lib/tdlib.pri)
include(lib/qmlapplicationviewer/qmlapplicationviewer.pri)
include(lib/warnings.pri)
qtcAddDeployment()

HEADERS           += \
    src/authentication/AuthenticationStore.hpp \
    src/chat/ChatModel.h \
    src/chat/ChatStore.hpp \
    src/client/Client.hpp \
    src/client/TdApi.hpp \
    src/util/Overloaded.hpp \
    src/util/QObjectConnect.hpp \
    src/util/QStringHash.hpp

SOURCES           += \
    src/authentication/AuthenticationStore.cpp \
    src/chat/ChatModel.cpp \
    src/chat/ChatStore.cpp \
    src/client/Client.cpp \
    src/client/TdApi.cpp \
    src/main.cpp

QMAKE_CXXFLAGS += -std=c++2a
QMAKE_LFLAGS += -Os -rdynamic

contains(MEEGO_EDITION, harmattan) {
    QMAKE_CC = /opt/strawberry-gcc-10.2/bin/arm-none-linux-gnueabi-gcc
    QMAKE_CXX = /opt/strawberry-gcc-10.2/bin/arm-none-linux-gnueabi-g++
    QMAKE_LINK = /opt/strawberry-gcc-10.2/bin/arm-none-linux-gnueabi-g++

    SOURCES += src/authentication/Account.cpp
    HEADERS += src/authentication/Account.hpp

    PKGCONFIG += AccountSetup accounts-qt
    QMAKE_LFLAGS += -static-libstdc++
}

QML_IMPORT_PATH = /opt/QtSDK/Simulator/Qt/gcc/imports

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

DISTFILES += \
    tg.Authorization.service
