#pragma once

#include <qglobal.h>
#include "reaper_p.h"

#include <extensionsystem/iplugin.h>
#include <utils/environment.h>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

namespace Utils {
class PathChooser;
}

namespace Core {
namespace Internal {

class HomeMode;
class MainWindow;

class CorePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.milab.Qt.MFDSPlugin" FILE "Core.json")

public:
    CorePlugin();
    ~CorePlugin() override;

    static CorePlugin *instance();

    bool initialize(const QStringList &arguments, QString *errorMessage = nullptr) override;
    void extensionsInitialized() override;
    bool delayedInitialize() override;
    ShutdownFlag aboutToShutdown() override;
    QObject *remoteCommand(const QStringList & /* options */,
                           const QString &workingDirectory,
                           const QStringList &args) override;

    static Utils::Environment startupSystemEnvironment();
    static Utils::EnvironmentItems environmentChanges();
    static void setEnvironmentChanges(const Utils::EnvironmentItems &changes);
    static QString msgCrashpadInformation();

public slots:
    void fileOpenRequest(const QString&);

private:
    static void addToPathChooserContextMenu(Utils::PathChooser *pathChooser, QMenu *menu);
    static void setupSystemEnvironment();
    void checkSettings();
    void warnAboutCrashReporing();

    MainWindow *m_mainWindow = nullptr;
    HomeMode *m_editMode = nullptr;
    ProcessReapers m_reaper;
    Utils::Environment m_startupSystemEnvironment;
    Utils::EnvironmentItems m_environmentChanges;
};

} // namespace Internal
} // namespace Core
