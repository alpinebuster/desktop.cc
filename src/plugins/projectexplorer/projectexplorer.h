#pragma once

#include "projectexplorer_export.h"

#include <extensionsystem/iplugin.h>

#include <QPair>

QT_BEGIN_NAMESPACE
class QPoint;
class QAction;
class QThreadPool;
QT_END_NAMESPACE

namespace Core {
class IMode;
//class OutputWindow;
} // namespace Core

namespace Utils {
class ProcessHandle;
class FilePath;
}

namespace ProjectExplorer {
//class Project;
//class Node;
//class FolderNode;
//class FileNode;

namespace Internal {
//class ProjectExplorerSettings;
}

class PROJECTEXPLORER_EXPORT ProjectExplorerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.milab.Qt.MFDSPlugin" FILE "ProjectExplorer.json")

    friend class ProjectExplorerPluginPrivate;

public:
    ProjectExplorerPlugin();
    ~ProjectExplorerPlugin() override;

    static ProjectExplorerPlugin *instance();

    static void openProject(const Utils::FilePath &filePath);
    static bool saveModifiedFiles();
    static void clearRecentProjects();
    static void removeFromRecentProjects(const QString &fileName, const QString &displayName);

    // PluginInterface
    bool initialize(const QStringList &arguments, QString *errorMessage) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

    static void showQtSettings();
    static void initiateInlineRenaming();
    static QThreadPool *sharedThreadPool();

    static void updateActions();
    static void closeProject(const QString &id);

signals:
    void finishedInitialization();

    // Is emitted when a project has been added/removed,
    // or the file list of a specific project has changed.
    void fileListChanged();
    void settingsChanged();
};

} // namespace ProjectExplorer
