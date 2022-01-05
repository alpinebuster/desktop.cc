include(../../mfds_plugin.pri)

HEADERS += projectexplorer.h \
    projectexplorer_export.h \
    projectexplorerconstants.h \
    foldernavigationwidget.h \
    headerpath.h \
    projectexplorericons.h

SOURCES += projectexplorer.cpp \
    projectexplorerconstants.cpp \
    foldernavigationwidget.cpp \
    projectexplorericons.cpp

journald {
    SOURCES += journaldwatcher.cpp
    HEADERS += journaldwatcher.h
    DEFINES += WITH_JOURNALD
    LIBS += -lsystemd
}

RESOURCES += projectexplorer.qrc

DEFINES += PROJECTEXPLORER_LIBRARY

!isEmpty(PROJECT_USER_FILE_EXTENSION) {
    DEFINES += PROJECT_USER_FILE_EXTENSION=$${PROJECT_USER_FILE_EXTENSION}
}
