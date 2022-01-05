#include "projectexplorer.h"

#ifdef WITH_JOURNALD
#include "journaldwatcher.h"
#endif
#include "foldernavigationwidget.h"
#include "projectexplorerconstants.h"
#include <app/app_version.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/diffservice.h>
#include <coreplugin/documentmanager.h>
#include <coreplugin/homemanager/documentmodel.h>
#include <coreplugin/homemanager/homemanager.h>
#include <coreplugin/fileutils.h>
#include <coreplugin/icore.h>
#include <coreplugin/idocument.h>
#include <coreplugin/idocumentfactory.h>
#include <coreplugin/imode.h>
#include <coreplugin/minisplitter.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/navigationwidget.h>
#include <coreplugin/outputpane.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <utils/algorithm.h>
#include <utils/fileutils.h>
#include <utils/macroexpander.h>
#include <utils/mimetypes/mimedatabase.h>
#include <utils/parameteraction.h>
#include <utils/processhandle.h>
#include <utils/proxyaction.h>
#include <utils/qtcassert.h>
#include <utils/removefiledialog.h>
#include <utils/stringutils.h>
#include <utils/utilsicons.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPair>
#include <QSettings>
#include <QThreadPool>
#include <QTimer>

#include <functional>
#include <memory>
#include <vector>

/*!
    \namespace ProjectExplorer
    The ProjectExplorer namespace contains the classes to explore projects.
*/

/*!
    \namespace ProjectExplorer::Internal
    The ProjectExplorer::Internal namespace is the internal namespace of the
    ProjectExplorer plugin.
    \internal
*/

/*!
    \class ProjectExplorer::ProjectExplorerPlugin

    \brief The ProjectExplorerPlugin class contains static accessor and utility
    functions to obtain the current project, open projects, and so on.
*/

using namespace Core;
using namespace ProjectExplorer::Internal;
using namespace Utils;

namespace ProjectExplorer {

namespace Constants {
const int  P_MODE_SESSION         = 85;

// Actions
const char NEWPROJECT[]           = "ProjectExplorer.NewProject";
const char LOAD[]                 = "ProjectExplorer.Load";
const char UNLOAD[]               = "ProjectExplorer.Unload";
const char ADDEXISTINGFILES[]     = "ProjectExplorer.AddExistingFiles";
const char ADDEXISTINGDIRECTORY[] = "ProjectExplorer.AddExistingDirectory";
const char ADDNEWSUBPROJECT[]     = "ProjectExplorer.AddNewSubproject";
const char REMOVEPROJECT[]        = "ProjectExplorer.RemoveProject";
const char OPENFILE[]             = "ProjectExplorer.OpenFile";
const char SEARCHONFILESYSTEM[]   = "ProjectExplorer.SearchOnFileSystem";
const char OPENTERMINALHERE[]     = "ProjectExplorer.OpenTerminalHere";
const char DUPLICATEFILE[]        = "ProjectExplorer.DuplicateFile";
const char DELETEFILE[]           = "ProjectExplorer.DeleteFile";
const char SETSTARTUP[]           = "ProjectExplorer.SetStartup";
const char PROJECTTREE_COLLAPSE_ALL[] = "ProjectExplorer.CollapseAll";
const char PROJECTTREE_EXPAND_ALL[] = "ProjectExplorer.ExpandAll";

// Action priorities
const int  P_ACTION_RUN            = 100;
const int  P_ACTION_BUILDPROJECT   = 80;

// Menus
const char M_RECENTPROJECTS[]     = "ProjectExplorer.Menu.Recent";
const char M_CLOSE_PROJECT[]     = "ProjectExplorer.Menu.Unload";

const char RUNMENUCONTEXTMENU[]   = "Project.RunMenu";
const char FOLDER_OPEN_LOCATIONS_CONTEXT_MENU[]   = "Project.F.OpenLocation.CtxMenu";
const char PROJECT_OPEN_LOCATIONS_CONTEXT_MENU[]  = "Project.P.OpenLocation.CtxMenu";

const char RECENTPROJECTS_FILE_NAMES_KEY[]        = "ProjectExplorer/RecentProjects/FileNames";
const char RECENTPROJECTS_DISPLAY_NAMES_KEY[]     = "ProjectExplorer/RecentProjects/DisplayNames";
const char AUTO_RESTORE_SESSION_SETTINGS_KEY[]    = "ProjectExplorer/Settings/AutoRestoreLastSession";

} // namespace Constants

class ProjectExplorerPluginPrivate : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(ProjectExplorer::ProjectExplorerPlugin)

public:
    ProjectExplorerPluginPrivate();

    void updateContextMenuActions();
    void updateLocationSubMenus();

    void addToRecentProjects(const QString &fileName, const QString &displayName);

    void updateActions();
    void updateContext();

    void loadAction();

    void savePersistentSettings();
    void updateRecentProjectMenu();
    void clearRecentProjects();
    void openRecentProject(const QString &fileName);
    void removeFromRecentProjects(const QString &fileName, const QString &displayName);
    void openProject();
    void updateCloseProjectMenu();

    void searchOnFileSystem();

    // TODO
    void addNewFile();
    void addExistingFiles(const FilePaths &filePaths);
    void duplicateFile();
    void deleteFile();
    void renameFile();

    void doUpdateRunActions();

    void checkForShutdown();
    void timerEvent(QTimerEvent *) override;

    QList<QPair<QString, QString> > recentProjects() const;

public:
    QMenu *m_sessionMenu;
    QMenu *m_openWithMenu;
    QMenu *m_openTerminalMenu;
    QAction *m_openProjectAction = nullptr;

    QMultiMap<int, QObject*> m_actionMap;
    QAction *m_addExistingFilesAction;
    QAction *m_addExistingDirectoryAction;
    QAction *m_addNewSubprojectAction;
    QAction *m_addExistingProjectsAction;
    QAction *m_removeProjectAction;
//    QAction *m_addNewFileAction;
//    QAction *m_duplicateFileAction;
//    QAction *m_deleteFileAction;
//    QAction *m_renameFileAction;
    QAction *m_filePropertiesAction = nullptr;
    QAction *m_diffFileAction;
    QAction *m_projectTreeCollapseAllAction;
    QAction *m_projectTreeExpandAllAction;
    QAction *m_projectTreeExpandNodeAction = nullptr;
    QAction *m_showInGraphicalShell;
    QAction *m_openTerminalHere;
    QAction *m_openTerminalHereBuildEnv;
    QAction *m_openTerminalHereRunEnv;
    Utils::ParameterAction *m_setStartupProjectAction;
    QAction *m_projectSelectorAction;
    QAction *m_projectSelectorActionMenu;
    QAction *m_projectSelectorActionQuick;
    QAction *m_runSubProject;

    QString m_sessionToRestoreAtStartup;

    QStringList m_profileMimeTypes;
    int m_activeRunControlCount = 0;
    int m_shutdownWatchDogId = -1;


    QList<QPair<QString, QString>> m_recentProjects; // pair of filename, displayname
    static const int m_maxRecentProjects = 25;

    QString m_lastOpenDirectory;

    QString m_projectFilterString;
    bool m_shouldHaveRunConfiguration = false;
    bool m_shuttingDown = false;

    QStringList m_arguments;

#ifdef WITH_JOURNALD
    JournaldWatcher m_journalWatcher;
#endif
    QThreadPool m_threadPool;
    FolderNavigationWidgetFactory m_folderNavigationWidgetFactory;

    IDocumentFactory m_documentFactory;
};

static ProjectExplorerPlugin *m_instance = nullptr;
static ProjectExplorerPluginPrivate *dd = nullptr;

ProjectExplorerPlugin::ProjectExplorerPlugin()
{
    m_instance = this;
}

ProjectExplorerPlugin::~ProjectExplorerPlugin()
{
    delete dd;
    dd = nullptr;
    m_instance = nullptr;
}

ProjectExplorerPlugin *ProjectExplorerPlugin::instance()
{
    return m_instance;
}

bool ProjectExplorerPlugin::initialize(const QStringList &arguments, QString *error)
{
    Q_UNUSED(arguments)
    Q_UNUSED(error)

    dd = new ProjectExplorerPluginPrivate;

    ActionContainer *mfile =
        ActionManager::actionContainer(Core::Constants::M_FILE);

    // context menu sub menus:
    ActionContainer *folderOpenLocationCtxMenu =
            ActionManager::createMenu(Constants::FOLDER_OPEN_LOCATIONS_CONTEXT_MENU);
    folderOpenLocationCtxMenu->menu()->setTitle(tr("Open..."));
    folderOpenLocationCtxMenu->setOnAllDisabledBehavior(ActionContainer::Hide);

    ActionContainer *projectOpenLocationCtxMenu =
            ActionManager::createMenu(Constants::PROJECT_OPEN_LOCATIONS_CONTEXT_MENU);
    projectOpenLocationCtxMenu->menu()->setTitle(tr("Open..."));
    projectOpenLocationCtxMenu->setOnAllDisabledBehavior(ActionContainer::Hide);

    ActionContainer *runMenu = ActionManager::createMenu(Constants::RUNMENUCONTEXTMENU);
    runMenu->setOnAllDisabledBehavior(ActionContainer::Hide);

    Command *cmd;
    mfile->addSeparator(Core::Constants::G_FILE_PROJECT);

    // Open Project Action
    QIcon icon = QIcon::fromTheme(QLatin1String("document-open-project"), Utils::Icons::PROJECT.icon());
    dd->m_openProjectAction = new QAction(icon, tr("&Open Project..."), this);
    cmd = ActionManager::registerAction(dd->m_openProjectAction, Core::Constants::OPEN_PROJECT);
    mfile->addAction(cmd, Core::Constants::G_FILE_OPEN);
    connect(dd->m_openProjectAction, &QAction::triggered, dd, &ProjectExplorerPluginPrivate::openProject);

    // recent projects menu
    ActionContainer *mrecent =
        ActionManager::createMenu(Constants::M_RECENTPROJECTS);
    mrecent->menu()->setTitle(tr("Recent P&rojects"));
    mrecent->setOnAllDisabledBehavior(ActionContainer::Show);
    mfile->addMenu(mrecent, Core::Constants::G_FILE_OPEN);
    connect(mfile->menu(), &QMenu::aboutToShow,
            dd, &ProjectExplorerPluginPrivate::updateRecentProjectMenu);

    ActionContainer *munload =
        ActionManager::createMenu(Constants::M_CLOSE_PROJECT);
    munload->menu()->setTitle(tr("Close Pro&ject"));
    munload->setOnAllDisabledBehavior(ActionContainer::Show);
    mfile->addMenu(munload, Core::Constants::G_FILE_PROJECT);
    connect(mfile->menu(), &QMenu::aboutToShow,
            dd, &ProjectExplorerPluginPrivate::updateCloseProjectMenu);

    connect(ICore::instance(), &ICore::saveSettingsRequested,
            dd, &ProjectExplorerPluginPrivate::savePersistentSettings);

    QSettings *s = ICore::settings();
    const QStringList fileNames = s->value(Constants::RECENTPROJECTS_FILE_NAMES_KEY).toStringList();
    const QStringList displayNames = s->value(Constants::RECENTPROJECTS_DISPLAY_NAMES_KEY)
                                         .toStringList();
    if (fileNames.size() == displayNames.size()) {
        for (int i = 0; i < fileNames.size(); ++i) {
            dd->m_recentProjects.append(qMakePair(fileNames.at(i), displayNames.at(i)));
        }
    }

//    connect(dd->m_addNewFileAction, &QAction::triggered,
//            dd, &ProjectExplorerPluginPrivate::addNewFile);
//    connect(dd->m_duplicateFileAction, &QAction::triggered,
//            dd, &ProjectExplorerPluginPrivate::duplicateFile);
//    connect(dd->m_deleteFileAction, &QAction::triggered,
//            dd, &ProjectExplorerPluginPrivate::deleteFile);
//    connect(dd->m_renameFileAction, &QAction::triggered,
//            dd, &ProjectExplorerPluginPrivate::renameFile);

    return true;
}

void ProjectExplorerPluginPrivate::loadAction()
{
    QString dir = dd->m_lastOpenDirectory;

    // for your special convenience, we preselect a pro file if it is
    // the current file
    if (const IDocument *document = HomeManager::currentDocument()) {
        const QString fn = document->filePath().toString();
        const bool isProject = dd->m_profileMimeTypes.contains(document->mimeType());
        dir = isProject ? fn : QFileInfo(fn).absolutePath();
    }

    QString filename = QFileDialog::getOpenFileName(ICore::dialogParent(),
                                                    tr("Load Project"), dir,
                                                    dd->m_projectFilterString);
    if (filename.isEmpty())
        return;

    updateActions();
}

void ProjectExplorerPlugin::extensionsInitialized()
{
    // Register factories for all project managers
    QStringList allGlobPatterns;

    const QString filterSeparator = QLatin1String(";;");
    QStringList filterStrings;

    dd->m_documentFactory.addMimeType(QStringLiteral("inode/directory"));

    QString allProjectsFilter = tr("All Projects");
    allProjectsFilter += QLatin1String(" (") + allGlobPatterns.join(QLatin1Char(' '))
            + QLatin1Char(')');
    filterStrings.prepend(allProjectsFilter);
    dd->m_projectFilterString = filterStrings.join(filterSeparator);

}

ExtensionSystem::IPlugin::ShutdownFlag ProjectExplorerPlugin::aboutToShutdown()
{
    dd->m_shuttingDown = true;

    // Attempt to synchronously shutdown all run controls.
    // If that fails, fall back to asynchronous shutdown (Debugger run controls
    // might shutdown asynchronously).
    if (dd->m_activeRunControlCount == 0)
        return SynchronousShutdown;

    return AsynchronousShutdown;
}

void ProjectExplorerPluginPrivate::savePersistentSettings()
{
    if (dd->m_shuttingDown)
        return;

    QtcSettings *s = ICore::settings();
    // FIXME - What's the purpose of this ?
    s->remove(QLatin1String("ProjectExplorer/RecentProjects/Files"));

    QStringList fileNames;
    QStringList displayNames;
    QList<QPair<QString, QString> >::const_iterator it, end;
    end = dd->m_recentProjects.constEnd();
    for (it = dd->m_recentProjects.constBegin(); it != end; ++it) {
        fileNames << (*it).first;
        displayNames << (*it).second;
    }
    s->setValueWithDefault(Constants::RECENTPROJECTS_FILE_NAMES_KEY, fileNames);
    s->setValueWithDefault(Constants::RECENTPROJECTS_DISPLAY_NAMES_KEY, displayNames);
}

QThreadPool *ProjectExplorerPlugin::sharedThreadPool()
{
    return &(dd->m_threadPool);
}

void ProjectExplorerPluginPrivate::checkForShutdown()
{
    --m_activeRunControlCount;
    QTC_ASSERT(m_activeRunControlCount >= 0, m_activeRunControlCount = 0);
    if (m_shuttingDown && m_activeRunControlCount == 0)
        emit m_instance->asynchronousShutdownFinished();
}

void ProjectExplorerPluginPrivate::timerEvent(QTimerEvent *ev)
{
    if (m_shutdownWatchDogId == ev->timerId())
        emit m_instance->asynchronousShutdownFinished();
}

void ProjectExplorerPlugin::initiateInlineRenaming()
{
    dd->renameFile();
}

void ProjectExplorerPlugin::openProject(const Utils::FilePath &filePath)
{
    if (!filePath.toString().isEmpty()) {
        QString prettyFileName(QDir::fromNativeSeparators(filePath.toString()));
        ModeManager::activateMode(Id(Core::Constants::MODE_HOME));
        NavigationWidget::activateSubWidget(Id(ProjectExplorer::Constants::FOLDER_NAVIGATION_WIDGET_FACTORY_ID), Side::Left);
        // FIXME - This may not work on MACOS or Linux
        QFileInfo fileinfo;
        fileinfo = QFileInfo(prettyFileName);
        QString rootDirectoryId = fileinfo.baseName();
        FolderNavigationWidgetFactory::insertRootDirectory({QLatin1String("A.%1").arg(rootDirectoryId),
                                                            FolderNavigationWidgetFactory::getRootDirectoryLength() /*sortValue*/,
                                                            FolderNavigationWidget::tr(prettyFileName.toLocal8Bit().data()),
                                                            Utils::FilePath::fromString(prettyFileName),
                                                            Utils::Icons::OPENFILE.icon()});
        FolderNavigationWidgetFactory::setCurrentProject(QLatin1String("A.%1").arg(rootDirectoryId));
        dd->addToRecentProjects(prettyFileName, rootDirectoryId);
        dd->updateActions();
    }
}

QList<QPair<QString, QString>> ProjectExplorerPluginPrivate::recentProjects() const
{
    return Utils::filtered(dd->m_recentProjects, [](const QPair<QString, QString> &p) {
        return QFileInfo(p.first).isDir();
    });
}

void ProjectExplorerPluginPrivate::updateCloseProjectMenu()
{
    ActionContainer *aci = ActionManager::actionContainer(Constants::M_CLOSE_PROJECT);
    QMenu *menu = aci->menu();
    menu->clear();
    // TODO - Get projects that are already open in the WORKING directory
    const QList<QString> idList = FolderNavigationWidgetFactory::getOpenedProjects();
    for (const QString &id : idList) {
        QAction *action = menu->addAction(tr("Close Project \"%1\"").arg(id.split(".")[1]));
        connect(action, &QAction::triggered,
                [id] { ProjectExplorerPlugin::closeProject(id); } );
    }
    menu->setEnabled(idList.size() > 0);
}

void ProjectExplorerPlugin::closeProject(const QString &id)
{
    // Remove project from WORKING directory
    FolderNavigationWidgetFactory::removeProject(id);
    dd->updateActions();
}

void ProjectExplorerPluginPrivate::updateRecentProjectMenu()
{
    using StringPairListConstIterator = QList<QPair<QString, QString> >::const_iterator;
    ActionContainer *aci = ActionManager::actionContainer(Constants::M_RECENTPROJECTS);
    QMenu *menu = aci->menu();
    menu->clear();

    int acceleratorKey = 1;
    auto projects = recentProjects();
    //projects (ignore sessions, they used to be in this list)
    const StringPairListConstIterator end = projects.constEnd();
    for (StringPairListConstIterator it = projects.constBegin(); it != end; ++it, ++acceleratorKey) {
        const QString fileName = it->first;
        if (fileName.endsWith(QLatin1String(".qws")))
            continue;

        const QString actionText = ActionManager::withNumberAccelerator(
                    Utils::withTildeHomePath(fileName), acceleratorKey);
        QAction *action = menu->addAction(actionText);
        connect(action, &QAction::triggered, this, [this, fileName] {
            openRecentProject(fileName);
        });
    }
    const bool hasRecentProjects = !projects.empty();
    menu->setEnabled(hasRecentProjects);

    // add the Clear Menu item
    if (hasRecentProjects) {
        menu->addSeparator();
        QAction *action = menu->addAction(QCoreApplication::translate(
                                          "Core", Core::Constants::TR_CLEAR_MENU));
        connect(action, &QAction::triggered,
                this, &ProjectExplorerPluginPrivate::clearRecentProjects);
    }
//    emit m_instance->recentProjectsChanged();
}

void ProjectExplorerPluginPrivate::clearRecentProjects()
{
    m_recentProjects.clear();
//    updateWelcomePage();
}

void ProjectExplorerPluginPrivate::openRecentProject(const QString &fileName)
{
    if (!fileName.isEmpty()) {
        QString prettyFileName(QDir::fromNativeSeparators(fileName));
        ModeManager::activateMode(Id(Core::Constants::MODE_HOME));
        NavigationWidget::activateSubWidget(Id(ProjectExplorer::Constants::FOLDER_NAVIGATION_WIDGET_FACTORY_ID), Side::Left);
        // FIXME - This may not work on MACOS or Linux
        QFileInfo fileinfo;
        fileinfo = QFileInfo(prettyFileName);
        QString rootDirectoryId = fileinfo.baseName();
        FolderNavigationWidgetFactory::insertRootDirectory({QLatin1String("A.%1").arg(rootDirectoryId),
                                                            FolderNavigationWidgetFactory::getRootDirectoryLength() /*sortValue*/,
                                                            FolderNavigationWidget::tr(prettyFileName.toLocal8Bit().data()),
                                                            Utils::FilePath::fromString(prettyFileName),
                                                            Utils::Icons::OPENFILE.icon()});
        FolderNavigationWidgetFactory::setCurrentProject(QLatin1String("A.%1").arg(rootDirectoryId));
        dd->addToRecentProjects(prettyFileName, rootDirectoryId);
        dd->updateActions();
    }
}

void ProjectExplorerPluginPrivate::removeFromRecentProjects(const QString &fileName,
                                                            const QString &displayName)
{
    QTC_ASSERT(!fileName.isEmpty() && !displayName.isEmpty(), return);
    QTC_CHECK(m_recentProjects.removeOne(QPair<QString, QString>(fileName, displayName)));
}

void ProjectExplorerPluginPrivate::updateActions()
{
    // TODO - Determine if there is an open project in the WORKING directory
    ActionContainer *aci =
        ActionManager::actionContainer(Constants::M_CLOSE_PROJECT);
    QMenu *menu = aci->menu();
    const QList<QString> idList = FolderNavigationWidgetFactory::getOpenedProjects();
    menu->setEnabled(idList.size() > 0);
    menu->menuAction()->setVisible(idList.size() > 0);
}

void ProjectExplorerPlugin::clearRecentProjects()
{
    dd->clearRecentProjects();
}

void ProjectExplorerPlugin::removeFromRecentProjects(const QString &fileName,
                                                     const QString &displayName)
{
    dd->removeFromRecentProjects(fileName, displayName);
}

bool ProjectExplorerPlugin::saveModifiedFiles()
{
    QList<IDocument *> documentsToSave = DocumentManager::modifiedDocuments();
    if (!documentsToSave.isEmpty()) {

    }
    return true;
}

//NBS handle case where there is no activeBuildConfiguration
// because someone delete all build configurations

ProjectExplorerPluginPrivate::ProjectExplorerPluginPrivate()
{

}

void ProjectExplorerPluginPrivate::addToRecentProjects(const QString &fileName, const QString &displayName)
{
    if (fileName.isEmpty())
        return;
    QString prettyFileName(QDir::fromNativeSeparators(fileName));

    QList<QPair<QString, QString> >::iterator it;
    for (it = m_recentProjects.begin(); it != m_recentProjects.end();)
        if ((*it).first == prettyFileName)
            it = m_recentProjects.erase(it);
        else
            ++it;

    if (m_recentProjects.count() > m_maxRecentProjects)
        m_recentProjects.removeLast();
    m_recentProjects.prepend(qMakePair(prettyFileName, displayName));
    QFileInfo fi(prettyFileName);
    m_lastOpenDirectory = fi.absolutePath();

    // Saving settings directly in a mode change is not a good idea, since the mode change
    // can be part of a bigger change. Save settings after that bigger change had a chance to
    // complete.
    QTimer::singleShot(0, ICore::instance(), [] { ICore::saveSettings(ICore::ModeChanged); });
    //ICore::saveSettings(ICore::ModeChanged);
}

void ProjectExplorerPluginPrivate::updateContextMenuActions()
{
    m_addExistingFilesAction->setEnabled(false);
    m_addExistingDirectoryAction->setEnabled(false);
//    m_addNewFileAction->setEnabled(false);
    m_addNewSubprojectAction->setEnabled(false);
    m_addExistingProjectsAction->setEnabled(false);
    m_removeProjectAction->setEnabled(false);
//    m_duplicateFileAction->setEnabled(false);
//    m_deleteFileAction->setEnabled(false);
//    m_renameFileAction->setEnabled(false);
    m_diffFileAction->setEnabled(false);
    m_addExistingFilesAction->setVisible(true);
    m_addExistingDirectoryAction->setVisible(true);
//    m_addNewFileAction->setVisible(true);
    m_addNewSubprojectAction->setVisible(true);
    m_addExistingProjectsAction->setVisible(true);
    m_removeProjectAction->setVisible(true);
//    m_duplicateFileAction->setVisible(false);
//    m_deleteFileAction->setVisible(true);
    m_diffFileAction->setVisible(DiffService::instance());

    m_openTerminalHere->setVisible(true);
    m_openTerminalHereBuildEnv->setVisible(false);
    m_openTerminalHereRunEnv->setVisible(false);

    m_showInGraphicalShell->setVisible(true);

    ActionContainer *runMenu = ActionManager::actionContainer(Constants::RUNMENUCONTEXTMENU);
    runMenu->menu()->clear();
    runMenu->menu()->menuAction()->setVisible(false);
}

void ProjectExplorerPluginPrivate::updateLocationSubMenus()
{
    static QList<QAction *> actions;
    qDeleteAll(actions); // This will also remove these actions from the menus!
    actions.clear();

    ActionContainer *projectMenuContainer
            = ActionManager::actionContainer(Constants::PROJECT_OPEN_LOCATIONS_CONTEXT_MENU);
    QMenu *projectMenu = projectMenuContainer->menu();
    QTC_CHECK(projectMenu->actions().isEmpty());

    ActionContainer *folderMenuContainer
            = ActionManager::actionContainer(Constants::FOLDER_OPEN_LOCATIONS_CONTEXT_MENU);
    QMenu *folderMenu = folderMenuContainer->menu();
    QTC_CHECK(folderMenu->actions().isEmpty());
}

void ProjectExplorerPluginPrivate::addNewFile()
{

}

void ProjectExplorerPluginPrivate::addExistingFiles(const FilePaths &filePaths)
{

}

void ProjectExplorerPluginPrivate::openProject()
{
    QString folderPath = HomeManager::getOpenFolderName();
    if (folderPath.isEmpty())
        return;
    QString prettyFileName(QDir::fromNativeSeparators(folderPath));
    // Swith to MODE_HOME
    ModeManager::activateMode(Id(Core::Constants::MODE_HOME));
    // Set navigation widget to WORKING
    NavigationWidget::activateSubWidget(Id(ProjectExplorer::Constants::FOLDER_NAVIGATION_WIDGET_FACTORY_ID), Side::Left);
    // Add selected folder to WORKING
    // FIXME - This may not work on MACOS or Linux
    QFileInfo fileinfo;
    fileinfo = QFileInfo(prettyFileName);
    QString rootDirectoryId = fileinfo.baseName();
    FolderNavigationWidgetFactory::insertRootDirectory({QLatin1String("A.%1").arg(rootDirectoryId),
                                                        FolderNavigationWidgetFactory::getRootDirectoryLength() /*sortValue*/,
                                                        FolderNavigationWidget::tr(prettyFileName.toLocal8Bit().data()),
                                                        Utils::FilePath::fromString(prettyFileName),
                                                        Utils::Icons::OPENFILE.icon()});
    // Set current WORKING project to 'folderPath'
    FolderNavigationWidgetFactory::setCurrentProject(QLatin1String("A.%1").arg(rootDirectoryId));
    // Add selected folder to Recent projects
    dd->addToRecentProjects(prettyFileName, rootDirectoryId);
    dd->updateActions();
}

void ProjectExplorerPluginPrivate::searchOnFileSystem()
{

}

void ProjectExplorerPluginPrivate::duplicateFile()
{

}

void ProjectExplorerPluginPrivate::deleteFile()
{

}

void ProjectExplorerPluginPrivate::renameFile()
{

}

void ProjectExplorerPlugin::updateActions()
{
    dd->updateActions();
}
} // namespace ProjectExplorer
