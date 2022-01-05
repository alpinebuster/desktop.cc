include(../../mfds.pri)

TEMPLATE  = subdirs

SUBDIRS   = \
    coreplugin \
    imageviewer \
    projectexplorer \
    support \
    welcome

#isEmpty(IDE_PACKAGE_MODE) {
#    SUBDIRS += \
#        helloworld
#}

for(p, SUBDIRS) {
    QTC_PLUGIN_DEPENDS =
    include($$p/$${p}_dependencies.pri)
    pv = $${p}.depends
    $$pv = $$QTC_PLUGIN_DEPENDS
}

QMAKE_EXTRA_TARGETS += deployqt # dummy
