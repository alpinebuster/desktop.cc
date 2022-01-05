#pragma once

#include <coreplugin/inavigationwidgetfactory.h>
#include <utils/fileutils.h>

#include <QIcon>
#include <QWidget>

namespace Core {
class IContext;
class IEditor;
}

namespace Utils {
class NavigationTreeView;
class FileCrumbLabel;
class QtcSettings;
}

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QFileSystemModel;
class QModelIndex;
class QSortFilterProxyModel;
QT_END_NAMESPACE

namespace ProjectExplorer {
namespace Internal {

class DelayedFileCrumbLabel;

class FolderNavigationWidgetFactory : public Core::INavigationWidgetFactory
{
    Q_OBJECT

public:
    struct RootDirectory {
        QString id;
        int sortValue;
        QString displayName;
        Utils::FilePath path;
        QIcon icon;
    };

    FolderNavigationWidgetFactory();

    Core::NavigationView createWidget() override;
    void saveSettings(Utils::QtcSettings *settings, int position, QWidget *widget) override;
    void restoreSettings(QSettings *settings, int position, QWidget *widget) override;

    static void insertRootDirectory(const RootDirectory &directory);
    static int getRootDirectoryLength();
    static void removeProject(const QString &id);
    static void removeRootDirectory(const QString &id);
    static void setCurrentProject(const QString &index);
    static QList<QString> getOpenedProjects();

signals:
    void rootDirectoryAdded(const RootDirectory &directory);
    void rootDirectoryRemoved(const QString &id);

private:
    static int rootIndex(const QString &id);
    void updateProjectsDirectoryRoot();
    void registerActions();

    static QVector<RootDirectory> m_rootDirectories;
};

class FolderNavigationWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool autoSynchronization READ autoSynchronization WRITE setAutoSynchronization)
public:
    explicit FolderNavigationWidget(QWidget *parent = nullptr);

    bool autoSynchronization() const;
    bool hiddenFilesFilter() const;
    bool isShowingBreadCrumbs() const;
    bool isShowingFoldersOnTop() const;

    void setAutoSynchronization(bool sync);
    void toggleAutoSynchronization();
    void setShowBreadCrumbs(bool show);
    void setShowFoldersOnTop(bool onTop);
    void setCurrentIndex(const int &index);

    void insertRootDirectory(const FolderNavigationWidgetFactory::RootDirectory &directory);
    void removeRootDirectory(const QString &id);

    void addNewItem();
    void editCurrentItem();
    void removeCurrentItem();

protected:
    void contextMenuEvent(QContextMenuEvent *ev) override;

private slots:
    void setCrumblePath(const Utils::FilePath &filePath);

private:
    bool rootAutoSynchronization() const;
    void setRootAutoSynchronization(bool sync);
    void setHiddenFilesFilter(bool filter);
    void selectBestRootForFile(const Utils::FilePath &filePath);
    void handleCurrentEditorChanged(Core::IEditor *editor);
    void selectFile(const Utils::FilePath &filePath);
    void setRootDirectory(const Utils::FilePath &directory);
    int bestRootForFile(const Utils::FilePath &filePath);
    void openItem(const QModelIndex &index);
    void createNewFolder(const QModelIndex &parent);

    Utils::NavigationTreeView *m_listView   = nullptr;
    QFileSystemModel *m_fileSystemModel     = nullptr;
    QSortFilterProxyModel *m_sortProxyModel = nullptr;
    QAction *m_filterHiddenFilesAction      = nullptr;
    QAction *m_showBreadCrumbsAction        = nullptr;
    QAction *m_showFoldersOnTopAction       = nullptr;
    bool m_autoSync                         = false;
    bool m_rootAutoSync                     = true;
    QToolButton *m_toggleSync               = nullptr;
    QToolButton *m_toggleRootSync           = nullptr;
    QComboBox *m_rootSelector               = nullptr;
    QWidget *m_crumbContainer               = nullptr;
    DelayedFileCrumbLabel *m_crumbLabel     = nullptr;

    // FolderNavigationWidgetFactory needs private members to build a menu
    friend class FolderNavigationWidgetFactory;
};

} // namespace Internal
} // namespace ProjectExplorer
