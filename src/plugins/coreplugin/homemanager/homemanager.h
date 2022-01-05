#pragma once

#include "../core_global.h"
#include "../idocument.h"

#include "documentmodel.h"
#include "ieditor.h"

#include "utils/link.h"
#include "utils/textfileformat.h"

#include <QList>
#include <QWidget>

#include <functional>

QT_FORWARD_DECLARE_CLASS(QMenu)

namespace Utils {
class MimeType;
}

namespace Core {

class IDocument;
class SearchResultItem;

namespace Internal {
class HomeManagerPrivate;
class MainWindow;
} // namespace Internal

class CORE_EXPORT HomeManagerPlaceHolder final : public QWidget
{
    Q_OBJECT
public:
    explicit HomeManagerPlaceHolder(QWidget *parent = nullptr);
    ~HomeManagerPlaceHolder() final;

protected:
    void showEvent(QShowEvent *event) override;
};

class CORE_EXPORT HomeManager : public QObject
{
    Q_OBJECT

public:
    using WindowTitleHandler = std::function<QString (const QString &)>;

    static HomeManager *instance();

    enum OpenEditorFlag {
        NoFlags = 0,
        DoNotChangeCurrentEditor = 1,
        IgnoreNavigationHistory = 2,
        DoNotMakeVisible = 4,
        OpenInOtherSplit = 8,
        DoNotSwitchToDesignMode = 16,
        DoNotSwitchToEditMode = 32,
        SwitchSplitIfAlreadyVisible = 64,
        DoNotRaise = 128
    };
    Q_DECLARE_FLAGS(OpenEditorFlags, OpenEditorFlag)

    static IEditor *openEditor(const Utils::FilePath &filePath,
                               Utils::Id editorId = {},
                               OpenEditorFlags flags = NoFlags,
                               bool *newEditor = nullptr);
    static IEditor *openEditorAt(const Utils::Link &link,
                                 Utils::Id editorId = {},
                                 OpenEditorFlags flags = NoFlags,
                                 bool *newEditor = nullptr);

    // Kept for a while for transition.
    static IEditor *openEditor(const QString &fileName, Utils::Id editorId = {},
        OpenEditorFlags flags = NoFlags, bool *newEditor = nullptr); // FIXME: Remove overload
    static IEditor *openEditorAt(const QString &fileName,  int line, int column = 0,
                                 Utils::Id editorId = {}, OpenEditorFlags flags = NoFlags,
                                 bool *newEditor = nullptr); // FIXME: Remove overload

    static IEditor *openEditorWithContents(Utils::Id editorId, QString *titlePattern = nullptr,
                                           const QByteArray &contents = QByteArray(),
                                           const QString &uniqueId = QString(),
                                           OpenEditorFlags flags = NoFlags);
    static bool skipOpeningBigTextFile(const Utils::FilePath &filePath);
    static void clearUniqueId(IDocument *document);

    static bool openExternalEditor(const Utils::FilePath &filePath, Utils::Id editorId);
    static void addCloseEditorListener(const std::function<bool(IEditor *)> &listener);

    static QStringList getOpenFileNames();
    static QString getOpenFolderName();

    static IDocument *currentDocument();
    static IEditor *currentEditor();
    static QList<IEditor *> visibleEditors();

    static void activateEditor(IEditor *editor, OpenEditorFlags flags = NoFlags);
    static void activateEditorForEntry(DocumentModel::Entry *entry, OpenEditorFlags flags = NoFlags);
    static IEditor *activateEditorForDocument(IDocument *document, OpenEditorFlags flags = NoFlags);

    static bool closeDocuments(const QList<IDocument *> &documents, bool askAboutModifiedEditors = true);
    static bool closeDocuments(const QList<DocumentModel::Entry *> &entries);
    static void closeOtherDocuments(IDocument *document);
    static bool closeAllDocuments();

    static void addCurrentPositionToNavigationHistory(const QByteArray &saveState = QByteArray());
    static void setLastEditLocation(const IEditor *editor);
    static void cutForwardNavigationHistory();

    static bool saveDocument(IDocument *document);

    static bool closeEditors(const QList<IEditor *> &editorsToClose, bool askAboutModifiedEditors = true);

    static QByteArray saveState();
    static bool restoreState(const QByteArray &state);
    static bool hasSplitter();

    static void showEditorStatusBar(const QString &id,
                                    const QString &infoText,
                                    const QString &buttonText = QString(),
                                    QObject *object = nullptr,
                                    const std::function<void()> &function = {});
    static void hideEditorStatusBar(const QString &id);

    static bool isAutoSaveFile(const QString &fileName);

    static QTextCodec *defaultTextCodec();

    static Utils::TextFileFormat::LineTerminationMode defaultLineEnding();

    static qint64 maxTextFileSize();

    static void setWindowTitleAdditionHandler(WindowTitleHandler handler);
    static void setSessionTitleHandler(WindowTitleHandler handler);
    static void setWindowTitleVcsTopicHandler(WindowTitleHandler handler);

    static void addSaveAndCloseEditorActions(QMenu *contextMenu, DocumentModel::Entry *entry,
                                             IEditor *editor = nullptr);
    static void addPinEditorActions(QMenu *contextMenu, DocumentModel::Entry *entry);
    static void addNativeDirAndOpenWithActions(QMenu *contextMenu, DocumentModel::Entry *entry);
    static void populateOpenWithMenu(QMenu *menu, const QString &fileName);

public: // for tests
    static IDocument::ReloadSetting reloadSetting();
    static void setReloadSetting(IDocument::ReloadSetting behavior);

signals:
    void currentEditorChanged(Core::IEditor *editor);
    void currentDocumentStateChanged();
    void documentStateChanged(Core::IDocument *document);
    void editorCreated(Core::IEditor *editor, const QString &fileName);
    void editorOpened(Core::IEditor *editor);
    void documentOpened(Core::IDocument *document);
    void editorAboutToClose(Core::IEditor *editor);
    void editorsClosed(QList<Core::IEditor *> editors);
    void documentClosed(Core::IDocument *document);
    void findOnFileSystemRequest(const QString &path);
    void openFileProperties(const Utils::FilePath &path);
    void aboutToSave(Core::IDocument *document);
    void saved(Core::IDocument *document);
    void autoSaved();
    void currentEditorAboutToChange(Core::IEditor *editor);

#ifdef WITH_TESTS
    void linkOpened();
#endif

public slots:
    static void saveDocument();
    static void saveDocumentAs();
    static void revertToSaved();
    static bool closeAllEditors(bool askAboutModifiedEditors = true);
    static void slotCloseCurrentEditorOrDocument();
    static void closeOtherDocuments();
    static void splitSideBySide();
    static void gotoOtherSplit();
    static void goBackInNavigationHistory();
    static void goForwardInNavigationHistory();
    static void updateWindowTitles();

private:
    explicit HomeManager(QObject *parent);
    ~HomeManager() override;

    friend class Core::Internal::MainWindow;
};

} // namespace Core

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::HomeManager::OpenEditorFlags)
