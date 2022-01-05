TEMPLATE = subdirs
SUBDIRS = mfds/static.pro

include(../mfds.pri)

linux {
    appdata = $$cat($$PWD/metainfo/org.milab.mfds.appdata.xml.cmakein, blob)
    appdata = $$replace(appdata, \\$\\{IDE_VERSION_DISPLAY\\}, $$QTCREATOR_DISPLAY_VERSION)
    appdata = $$replace(appdata, \\$\\{DATE_ATTRIBUTE\\}, "")
    write_file($$OUT_PWD/metainfo/org.milab.mfds.appdata.xml, appdata)

    appstream.files = $$OUT_PWD/metainfo/org.milab.mfds.appdata.xml
    appstream.path = $$QTC_PREFIX/share/metainfo/

    desktop.files = share/applications/org.milab.mfds.desktop
    desktop.path = $$QTC_PREFIX/share/applications/

    INSTALLS += appstream desktop
}

defineTest(hasLupdate) {
    cmd = $$eval(QT_TOOL.lupdate.binary)
    isEmpty(cmd) {
        cmd = $$[QT_HOST_BINS]/lupdate
        contains(QMAKE_HOST.os, Windows):exists($${cmd}.exe): return(true)
        contains(QMAKE_HOST.os, Darwin):exists($${cmd}.app/Contents/MacOS/lupdate): return(true)
        exists($$cmd): return(true)
    } else {
        exists($$last(cmd)): return(true)
    }
    return(false)
}

# Translation folder
hasLupdate(): SUBDIRS += mfds/translations

DISTFILES += share.qbs \
    ../src/share/share.qbs
