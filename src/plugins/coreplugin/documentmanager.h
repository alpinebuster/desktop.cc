#pragma once

#include <coreplugin/core_global.h>

#include <utils/fileutils.h>
#include <utils/id.h>

#include <QObject>
#include <QPair>

namespace Utils { class FilePath; }

namespace Core {

class IDocument;

namespace Internal {
class DocumentManagerPrivate;
class MainWindow;
}

class CORE_EXPORT DocumentManager : public QObject
{
    Q_OBJECT
public:
    enum ResolveMode {
        ResolveLinks,
        KeepLinks
    };

    using RecentFile = QPair<Utils::FilePath, Utils::Id>;

    static DocumentManager *instance();

    // file pool to monitor
    static void addDocuments(const QList<IDocument *> &documents, bool addWatcher = true);
    static void addDocument(IDocument *document, bool addWatcher = true);
    static bool removeDocument(IDocument *document);
    static QList<IDocument *> modifiedDocuments();

    static void renamedFile(const Utils::FilePath &from, const Utils::FilePath &to);

    static void expectFileChange(const Utils::FilePath &filePath);
    static void unexpectFileChange(const Utils::FilePath &filePath);

    // recent files
    static void addToRecentFiles(const Utils::FilePath &filePath, Utils::Id editorId = {});
    Q_SLOT void clearRecentFiles();
    static QList<RecentFile> recentFiles();

    static void saveSettings();

    // helper functions
    static Utils::FilePath filePathKey(const Utils::FilePath &filePath, ResolveMode resolveMode);

    static bool saveDocument(IDocument *document,
                             const Utils::FilePath &filePath = Utils::FilePath(),
                             bool *isReadOnly = nullptr);

    static QString allDocumentFactoryFiltersString(QString *allFilesFilter);

    static QStringList getOpenFileNames(const QString &filters,
                                        const QString &path = QString(),
                                        QString *selectedFilter = nullptr);
    static QString getOpenFolderName(const QString &path = QString());
    static QString getSaveFileName(const QString &title,
                                   const QString &pathIn,
                                   const QString &filter = QString(),
                                   QString *selectedFilter = nullptr);
    static QString getSaveFileNameWithExtension(const QString &title, const QString &pathIn,
                                         const QString &filter);
    static QString getSaveAsFileName(const IDocument *document);

    static bool saveAllModifiedDocumentsSilently(bool *canceled = nullptr,
                                                 QList<IDocument *> *failedToClose = nullptr);
    static bool saveModifiedDocumentsSilently(const QList<IDocument *> &documents,
                                              bool *canceled = nullptr,
                                              QList<IDocument *> *failedToClose = nullptr);
    static bool saveModifiedDocumentSilently(IDocument *document,
                                             bool *canceled = nullptr,
                                             QList<IDocument *> *failedToClose = nullptr);

    static bool saveAllModifiedDocuments(const QString &message = QString(),
                                         bool *canceled = nullptr,
                                         const QString &alwaysSaveMessage = QString(),
                                         bool *alwaysSave = nullptr,
                                         QList<IDocument *> *failedToClose = nullptr);
    static bool saveModifiedDocuments(const QList<IDocument *> &documents,
                                      const QString &message = QString(),
                                      bool *canceled = nullptr,
                                      const QString &alwaysSaveMessage = QString(),
                                      bool *alwaysSave = nullptr,
                                      QList<IDocument *> *failedToClose = nullptr);
    static bool saveModifiedDocument(IDocument *document,
                                     const QString &message = QString(),
                                     bool *canceled = nullptr,
                                     const QString &alwaysSaveMessage = QString(),
                                     bool *alwaysSave = nullptr,
                                     QList<IDocument *> *failedToClose = nullptr);
    static void showFilePropertiesDialog(const Utils::FilePath &filePath);

    static QString fileDialogLastVisitedDirectory();
    static void setFileDialogLastVisitedDirectory(const QString &);

    static QString fileDialogInitialDirectory();

    static QString defaultLocationForNewFiles();
    static void setDefaultLocationForNewFiles(const QString &location);

    static bool useProjectsDirectory();
    static void setUseProjectsDirectory(bool);

    static Utils::FilePath projectsDirectory();
    static void setProjectsDirectory(const Utils::FilePath &directory);

    /* Used to notify e.g. the code model to update the given files. Does *not*
       lead to any editors to reload or any other editor manager actions. */
    static void notifyFilesChangedInternally(const Utils::FilePaths &filePaths);

signals:
    /* Used to notify e.g. the code model to update the given files. Does *not*
       lead to any editors to reload or any other editor manager actions. */
    void filesChangedInternally(const Utils::FilePaths &filePaths);
    /// emitted if all documents changed their name e.g. due to the file changing on disk
    void allDocumentsRenamed(const Utils::FilePath &from, const Utils::FilePath &to);
    /// emitted if one document changed its name e.g. due to save as
    void documentRenamed(Core::IDocument *document,
                         const Utils::FilePath &from,
                         const Utils::FilePath &to);
    void projectsDirectoryChanged(const Utils::FilePath &directory);

private:
    explicit DocumentManager(QObject *parent);
    ~DocumentManager() override;

    void documentDestroyed(QObject *obj);
    void checkForNewFileName();
    void checkForReload();
    void changedFile(const QString &file);
    void filePathChanged(const Utils::FilePath &oldName, const Utils::FilePath &newName);
    void updateSaveAll();
    static void registerSaveAllAction();

    friend class Core::Internal::MainWindow;
    friend class Core::Internal::DocumentManagerPrivate;
};

class CORE_EXPORT FileChangeBlocker
{
public:
    explicit FileChangeBlocker(const Utils::FilePath &filePath);
    ~FileChangeBlocker();
private:
    const Utils::FilePath m_filePath;
    Q_DISABLE_COPY(FileChangeBlocker)
};

} // namespace Core

Q_DECLARE_METATYPE(Core::DocumentManager::RecentFile)
