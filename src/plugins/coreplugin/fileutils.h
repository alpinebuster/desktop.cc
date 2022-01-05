#pragma once

#include "core_global.h"

#include <utils/fileutils.h>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace Utils { class Environment; }

namespace Core {

enum class HandleIncludeGuards { No, Yes };

struct CORE_EXPORT FileUtils
{
    // Helpers for common directory browser options.
    static void showInGraphicalShell(QWidget *parent, const QString &path);
    static void openTerminal(const QString &path);
    static void openTerminal(const QString &path, const Utils::Environment &env);
    static QString msgFindInDirectory();
    // Platform-dependent action descriptions
    static QString msgGraphicalShellAction();
    static QString msgTerminalHereAction();
    static QString msgTerminalWithAction();
    // File operations aware of version control and file system case-insensitiveness
    static void removeFile(const QString &filePath, bool deleteFromFS);
    static void removeFiles(const Utils::FilePaths &filePaths, bool deleteFromFS);
    static bool renameFile(const Utils::FilePath &from, const Utils::FilePath &to,
                           HandleIncludeGuards handleGuards = HandleIncludeGuards::No);
    // This method is used to refactor the include guards in the renamed headers
    static bool updateHeaderFileGuardAfterRename(const QString &headerPath, const QString &oldHeaderBaseName);
};

} // namespace Core
