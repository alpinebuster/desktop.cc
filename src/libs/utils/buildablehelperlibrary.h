#pragma once

#include "environment.h"
#include "fileutils.h"

QT_FORWARD_DECLARE_CLASS(QFileInfo)

namespace Utils {

class QTCREATOR_UTILS_EXPORT BuildableHelperLibrary
{
public:
    // returns the full path to the first qmake, qmake-qt4, qmake4 that has
    // at least version 2.0.0 and thus is a qt4 qmake
    static FilePath findSystemQt(const Environment &env);
    static FilePaths findQtsInEnvironment(const Environment &env, int maxCount = -1);
    static bool isQtChooser(const FilePath &filePath);
    static FilePath qtChooserToQmakePath(const FilePath &path);
    // return true if the qmake at qmakePath is a Qt (used by QtVersion)
    static QString qtVersionForQMake(const FilePath &qmakePath);
    // returns something like qmake4, qmake, qmake-qt4 or whatever distributions have chosen (used by QtVersion)
    static QStringList possibleQMakeCommands();
    static QString filterForQmakeFileDialog();

    static QString byInstallDataHelper(const QString &sourcePath,
                                       const QStringList &sourceFileNames,
                                       const QStringList &installDirectories,
                                       const QStringList &validBinaryFilenames,
                                       bool acceptOutdatedHelper);

    static bool copyFiles(const QString &sourcePath, const QStringList &files,
                          const QString &targetDirectory, QString *errorMessage);

    struct BuildHelperArguments {
        QString helperName;
        QString directory;
        Environment environment;

        FilePath qmakeCommand;
        QString targetMode;
        FilePath mkspec;
        QString proFilename;
        QStringList qmakeArguments;

        QString makeCommand;
        QStringList makeArguments;
    };

    static bool buildHelper(const BuildHelperArguments &arguments,
                            QString *log, QString *errorMessage);

    static bool getHelperFileInfoFor(const QStringList &validBinaryFilenames,
                                     const QString &directory, QFileInfo* info);
};

}
