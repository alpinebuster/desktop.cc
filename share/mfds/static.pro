TEMPLATE = aux

include(../../mfds.pri)

STATIC_BASE = $$PWD
STATIC_OUTPUT_BASE = $$IDE_DATA_PATH
STATIC_INSTALL_BASE = $$INSTALL_DATA_PATH

DATA_DIRS = \
    templates \
    themes \
    styles
macx: DATA_DIRS += scripts

for(data_dir, DATA_DIRS) {
    files = $$files($$PWD/$$data_dir/*, true)
    # Info.plist.in are handled below
    for(file, files):!contains(file, ".*/Info\\.plist\\.in$"):!contains(file, ".*__pycache__.*"):!exists($$file/*): \
        STATIC_FILES += $$file
}

include(../../mfds_data.pri)
