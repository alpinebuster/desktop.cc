#pragma once

#include "utils_global.h"

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace Utils {

class QTCREATOR_UTILS_EXPORT UnixUtils
{
public:
    static QString defaultFileBrowser();
    static QString fileBrowser(const QSettings *settings);
    static void setFileBrowser(QSettings *settings, const QString &term);
    static QString fileBrowserHelpText();
    static QString substituteFileBrowserParameters(const QString &command,
                                                   const QString &file);
};

}
