DEFINES += SUPPORT_LIBRARY
QT      += network

include(../../mfds_plugin.pri)

DEFINES += QMAKE_LIBRARY

HEADERS += \
    gettingstartedwelcomepage.h \
    qtsupport_global.h \
    qtsupportconstants.h \
    exampleslistmodel.h \
    screenshotcropper.h \
    supportplugin.h

SOURCES += \
    gettingstartedwelcomepage.cpp \
    exampleslistmodel.cpp \
    screenshotcropper.cpp \
    supportplugin.cpp

RESOURCES += \
    support.qrc
