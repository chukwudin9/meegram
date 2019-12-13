QT                += core declarative

INCLUDEPATH       += src/
TARGET             = meegram
TEMPLATE           = app
CONFIG            += link_pkgconfig
PKGCONFIG         +=

target.path        = /opt/meegram/bin
INSTALLS          += target

HEADERS           += \
    src/Controller.hpp

SOURCES           += \
    src/Controller.cpp \
    src/main.cpp

LIBS += -ltdjson
