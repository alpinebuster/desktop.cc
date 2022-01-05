#pragma once

#include "utils_global.h"

#include <QTemporaryDir>

namespace Utils {

class FilePath;

class QTCREATOR_UTILS_EXPORT TemporaryDirectory : public QTemporaryDir
{
public:
    explicit TemporaryDirectory(const QString &pattern);

    static QTemporaryDir *masterTemporaryDirectory();
    static void setMasterTemporaryDirectory(const QString &pattern);
    static QString masterDirectoryPath();

    FilePath path() const;
    FilePath filePath(const QString &fileName) const;
};

} // namespace Utils
