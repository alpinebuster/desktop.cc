DEFINES += CORE_LIBRARY
QT += \
    network \
    printsupport \
    qml \
    sql

# embedding build time information prevents repeatedly binary exact versions from same source code
isEmpty(QTC_SHOW_BUILD_DATE): QTC_SHOW_BUILD_DATE = $$(QTC_SHOW_BUILD_DATE)
!isEmpty(QTC_SHOW_BUILD_DATE): DEFINES += QTC_SHOW_BUILD_DATE

include(../../mfds_plugin.pri)
msvc: QMAKE_CXXFLAGS += -wd4251 -wd4290 -wd4250
SOURCES += corejsextensions.cpp \
    homemanager/homearea.cpp \
    homemanager/homemanager.cpp \
    homemanager/homeview.cpp \
    homemanager/homewindow.cpp \
    homemanager/ieditor.cpp \
    homemanager/ieditorfactory.cpp \
    homemanager/iexternalhome.cpp \
    homemanager/documentmodel.cpp \
    homemanager/openeditorsview.cpp \
    homemanager/openeditorswindow.cpp \
    homemanager/systemeditor.cpp \
    homemode.cpp \
    mainwindow.cpp \
    shellcommand.cpp \
    iwizardfactory.cpp \
    fancyactionbar.cpp \
    fancytabwidget.cpp \
    generalsettings.cpp \
    themechooser.cpp \
    icontext.cpp \
    jsexpander.cpp \
    outputpane.cpp \
    outputwindow.cpp \
    statusbarmanager.cpp \
    versiondialog.cpp \
    actionmanager/actionmanager.cpp \
    actionmanager/command.cpp \
    actionmanager/commandbutton.cpp \
    actionmanager/actioncontainer.cpp \
    actionmanager/commandsfile.cpp \
    dialogs/saveitemsdialog.cpp \
    dialogs/newdialog.cpp \
    dialogs/settingsdialog.cpp \
    actionmanager/commandmappings.cpp \
    dialogs/shortcutsettings.cpp \
    dialogs/readonlyfilesdialog.cpp \
    dialogs/openwithdialog.cpp \
    progressmanager/progressmanager.cpp \
    progressmanager/progressview.cpp \
    progressmanager/progressbar.cpp \
    progressmanager/futureprogress.cpp \
    reaper.cpp \
    coreplugin.cpp \
    modemanager.cpp \
    basefilewizard.cpp \
    basefilewizardfactory.cpp \
    generatedfile.cpp \
    plugindialog.cpp \
    inavigationwidgetfactory.cpp \
    navigationwidget.cpp \
    manhattanstyle.cpp \
    minisplitter.cpp \
    styleanimator.cpp \
    rightpane.cpp \
    sidebar.cpp \
    fileiconprovider.cpp \
    icore.cpp \
    dialogs/ioptionspage.cpp \
    settingsdatabase.cpp \
    imode.cpp \
    designmode.cpp \
    editortoolbar.cpp \
    helpitem.cpp \
    helpmanager.cpp \
    outputpanemanager.cpp \
    navigationsubwidget.cpp \
    sidebarwidget.cpp \
    dialogs/filepropertiesdialog.cpp \
    dialogs/promptoverwritedialog.cpp \
    fileutils.cpp \
    featureprovider.cpp \
    idocument.cpp \
    idocumentfactory.cpp \
    textdocument.cpp \
    documentmanager.cpp \
    dialogs/restartdialog.cpp \
    ioutputpane.cpp \
    windowsupport.cpp \
    opendocumentstreeview.cpp \
    messagebox.cpp \
    iwelcomepage.cpp \
    systemsettings.cpp \
    coreicons.cpp \
    diffservice.cpp \
    welcomepagehelper.cpp \
    dialogs/codecselector.cpp \
    plugininstallwizard.cpp

HEADERS += corejsextensions.h \
    homemanager/homearea.h \
    homemanager/homemanager.h \
    homemanager/homemanager_p.h \
    homemanager/homeview.h \
    homemanager/homewindow.h \
    homemanager/ieditor.h \
    homemanager/ieditorfactory.h \
    homemanager/ieditorfactory_p.h \
    homemanager/iexternalhome.h \
    homemanager/documentmodel.h \
    homemanager/openeditorsview.h \
    homemanager/openeditorswindow.h \
    homemanager/systemeditor.h \
    homemanager/documentmodel_p.h \
    homemode.h \
    mainwindow.h \
    shellcommand.h \
    iwizardfactory.h \
    fancyactionbar.h \
    fancytabwidget.h \
    generalsettings.h \
    themechooser.h \
    jsexpander.h \
    outputpane.h \
    outputwindow.h \
    statusbarmanager.h \
    actionmanager/actioncontainer.h \
    actionmanager/actionmanager.h \
    actionmanager/command.h \
    actionmanager/commandbutton.h \
    actionmanager/actionmanager_p.h \
    actionmanager/command_p.h \
    actionmanager/actioncontainer_p.h \
    actionmanager/commandsfile.h \
    dialogs/saveitemsdialog.h \
    dialogs/newdialog.h \
    dialogs/settingsdialog.h \
    actionmanager/commandmappings.h \
    dialogs/readonlyfilesdialog.h \
    dialogs/shortcutsettings.h \
    dialogs/openwithdialog.h \
    dialogs/ioptionspage.h \
    progressmanager/progressmanager_p.h \
    progressmanager/progressview.h \
    progressmanager/progressbar.h \
    progressmanager/futureprogress.h \
    progressmanager/progressmanager.h \
    reaper.h \
    reaper_p.h \
    icontext.h \
    icore.h \
    imode.h \
    ioutputpane.h \
    coreconstants.h \
    ifilewizardextension.h \
    versiondialog.h \
    core_global.h \
    coreplugin.h \
    modemanager.h \
    basefilewizard.h \
    basefilewizardfactory.h \
    generatedfile.h \
    plugindialog.h \
    inavigationwidgetfactory.h \
    navigationwidget.h \
    manhattanstyle.h \
    minisplitter.h \
    styleanimator.h \
    rightpane.h \
    sidebar.h \
    fileiconprovider.h \
    settingsdatabase.h \
    designmode.h \
    editortoolbar.h \
    helpitem.h \
    helpmanager.h \
    helpmanager_implementation.h \
    outputpanemanager.h \
    navigationsubwidget.h \
    sidebarwidget.h \
    dialogs/filepropertiesdialog.h \
    dialogs/promptoverwritedialog.h \
    fileutils.h \
    generatedfile.h \
    featureprovider.h \
    idocument.h \
    idocumentfactory.h \
    textdocument.h \
    documentmanager.h \
    dialogs/restartdialog.h \
    windowsupport.h \
    opendocumentstreeview.h \
    messagebox.h \
    iwelcomepage.h \
    systemsettings.h \
    coreicons.h \
    diffservice.h \
    welcomepagehelper.h \
    dialogs/codecselector.h \
    plugininstallwizard.h

FORMS += dialogs/newdialog.ui \
    dialogs/saveitemsdialog.ui \
    dialogs/readonlyfilesdialog.ui \
    dialogs/openwithdialog.ui \
    generalsettings.ui \
    dialogs/filepropertiesdialog.ui \
    systemsettings.ui

RESOURCES += core.qrc \
    fancyactionbar.qrc

win32 {
    SOURCES += progressmanager/progressmanager_win.cpp
    QT += gui-private # Uses QPlatformNativeInterface.
    LIBS += -lole32 -luser32
}
else:macx {
    OBJECTIVE_SOURCES += \
        progressmanager/progressmanager_mac.mm
    LIBS += -framework AppKit
}
else:unix {
    SOURCES += progressmanager/progressmanager_x11.cpp

    IMAGE_SIZE_LIST = 16 24 32 48 64 128 256 512

    for(imagesize, IMAGE_SIZE_LIST) {
        eval(image$${imagesize}.files = images/logo/$${imagesize}/QtProject-qtcreator.png)
        eval(image$${imagesize}.path = $$QTC_PREFIX/share/icons/hicolor/$${imagesize}x$${imagesize}/apps)
        INSTALLS += image$${imagesize}
    }
}

equals(TEST, 1) {
    SOURCES += testdatadir.cpp
    HEADERS += testdatadir.h
}
