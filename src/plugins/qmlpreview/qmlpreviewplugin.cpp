// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "qmlpreviewplugin.h"

#include "qmlpreviewruncontrol.h"
#include "qmlpreviewtr.h"

#ifdef WITH_TESTS
#include "tests/qmlpreviewclient_test.h"
#include "tests/qmlpreviewplugin_test.h"
#endif

#include <android/androidconstants.h>

#include <coreplugin/icore.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/messagemanager.h>

#include <extensionsystem/pluginmanager.h>

#include <projectexplorer/devicesupport/devicekitaspects.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/target.h>

#include <qmljs/qmljsdocument.h>
#include <qmljs/qmljsmodelmanagerinterface.h>

#include <qmlprojectmanager/qmlmultilanguageaspect.h>

#include <qtsupport/qtkitaspect.h>
#include <qtsupport/qtversionmanager.h>
#include <qtsupport/baseqtversion.h>

#include <solutions/tasking/tasktreerunner.h>

#include <texteditor/texteditor.h>

#include <utils/async.h>
#include <utils/icon.h>
#include <utils/mimeconstants.h>
#include <utils/proxyaction.h>

#include <QAction>
#include <QMessageBox>
#include <QPointer>
#include <QTimer>
#include <QToolBar>

using namespace ProjectExplorer;
using namespace Tasking;

namespace QmlPreview {

static QByteArray defaultFileLoader(const QString &filename, bool *success)
{
    if (Core::DocumentModel::Entry *entry
            = Core::DocumentModel::entryForFilePath(Utils::FilePath::fromString(filename))) {
        if (!entry->isSuspended) {
            *success = true;
            return entry->document->contents();
        }
    }

    QFile file(filename);
    *success = file.open(QIODevice::ReadOnly);
    return (*success) ? file.readAll() : QByteArray();
}

static bool defaultFileClassifier(const QString &filename)
{
    // We cannot dynamically load changes in qtquickcontrols2.conf
    return !filename.endsWith("qtquickcontrols2.conf");
}

static void defaultFpsHandler(quint16 frames[8])
{
    Core::MessageManager::writeSilently(QString::fromLatin1("QML preview: %1 fps").arg(frames[0]));
}

static std::unique_ptr<QmlDebugTranslationClient> defaultCreateDebugTranslationClientMethod(QmlDebug::QmlDebugConnection *connection)
{
    auto client = std::make_unique<QmlPreview::QmlDebugTranslationClient>(connection);
    return client;
}

class QmlPreviewPluginPrivate : public QObject
{
public:
    explicit QmlPreviewPluginPrivate(QmlPreviewPlugin *parent);

    void onEditorChanged(Core::IEditor *editor);
    void onEditorAboutToClose(Core::IEditor *editor);
    void setDirty();
    void attachToEditorManager();
    void detachFromEditorManager();
    void checkEditor();
    void checkFile(const QString &fileName);
    void triggerPreview(const QString &changedFile, const QByteArray &contents);
    void checkDocument(const QString &name, const QByteArray &contents, QmlJS::Dialect::Enum dialect);

    QmlPreviewPlugin *q = nullptr;
    QString m_previewedFile;
    QPointer<Core::IEditor> m_lastEditor;
    QmlPreviewRunControlList m_runningPreviews;
    bool m_dirty = false;
    QString m_localeIsoCode;

    LocalQmlPreviewSupportFactory localRunWorkerFactory;

    QmlPreviewRunnerSetting m_settings;
    QmlPreviewRunWorkerFactory runWorkerFactory;

    TaskTreeRunner m_parseRunner;
};

QmlPreviewPluginPrivate::QmlPreviewPluginPrivate(QmlPreviewPlugin *parent)
    : q(parent)
{
    m_settings.fileLoader = &defaultFileLoader;
    m_settings.fileClassifier = &defaultFileClassifier;
    m_settings.fpsHandler = &defaultFpsHandler;
    m_settings.createDebugTranslationClientMethod = &defaultCreateDebugTranslationClientMethod;

    Core::ActionContainer *menu = Core::ActionManager::actionContainer(
                Constants::M_BUILDPROJECT);
    QAction *runPreviewAction = new QAction(Tr::tr("QML Preview"), this);
    runPreviewAction->setToolTip(Tr::tr("Preview changes to QML code live in your application."));
    runPreviewAction->setEnabled(ProjectManager::startupProject() != nullptr);
    connect(ProjectManager::instance(), &ProjectManager::startupProjectChanged, runPreviewAction,
            &QAction::setEnabled);
    connect(runPreviewAction, &QAction::triggered, this, [&, runPreviewAction] {
        runPreviewAction->setEnabled(false);
        if (auto multiLanguageAspect = QmlProjectManager::QmlMultiLanguageAspect::current())
            m_localeIsoCode = multiLanguageAspect->currentLocale();
        bool skipDeploy = false;
        if (const Kit *kit = activeKitForActiveProject()) {
            skipDeploy = kit->supportedPlatforms().contains(Android::Constants::ANDROID_DEVICE_TYPE)
                || RunDeviceTypeKitAspect::deviceTypeId(kit) == Android::Constants::ANDROID_DEVICE_TYPE;
        }
        ProjectExplorerPlugin::runStartupProject(Constants::QML_PREVIEW_RUN_MODE, skipDeploy);
    });
    menu->addAction(
        Core::ActionManager::registerAction(runPreviewAction, "QmlPreview.RunPreview"),
        Constants::G_BUILD_RUN);

    menu = Core::ActionManager::actionContainer(Constants::M_FILECONTEXT);
    QAction *previewFileAction = new QAction(Tr::tr("Preview File"), this);
    connect(previewFileAction, &QAction::triggered, q, &QmlPreviewPlugin::previewCurrentFile);
    menu->addAction(
        Core::ActionManager::registerAction(previewFileAction, "QmlPreview.PreviewFile",
                                            Core::Context(Constants::C_PROJECT_TREE)),
        Constants::G_FILE_OTHER);
    previewFileAction->setVisible(false);
    connect(ProjectTree::instance(), &ProjectTree::currentNodeChanged, previewFileAction,
            [previewFileAction] (Node *node) {
                const FileNode *fileNode = node ? node->asFileNode() : nullptr;
                previewFileAction->setVisible(fileNode && fileNode->fileType() == FileType::QML);
            });
    connect(Core::EditorManager::instance(), &Core::EditorManager::editorOpened, this,
            [] (Core::IEditor *editor) {
        if (!editor)
            return;
        if (!editor->document())
            return;

        if (const QString mimeType = editor->document()->mimeType();
            mimeType != Utils::Constants::QML_MIMETYPE
            && mimeType != Utils::Constants::QMLUI_MIMETYPE) {
            return;
        }

        auto *textEditor = qobject_cast<TextEditor::BaseTextEditor *>(editor);
        if (!textEditor)
            return;
        TextEditor::TextEditorWidget *widget = textEditor->editorWidget();
        if (!widget)
            return;
        QToolBar *toolBar = widget->toolBar();
        if (!toolBar)
            return;

        const QIcon icon = Utils::Icon({
            {":/utils/images/run_small.png", Utils::Theme::IconsRunToolBarColor},
            {":/utils/images/eyeoverlay.png", Utils::Theme::IconsDebugColor}
        }).icon();
        Utils::ProxyAction *action = Utils::ProxyAction::proxyActionWithIcon(
                    Core::ActionManager::command("QmlPreview.RunPreview")->action(), icon);
        toolBar->insertAction(nullptr, action);
    });

    connect(q, &QmlPreviewPlugin::previewedFileChanged, this, &QmlPreviewPluginPrivate::checkFile);
}

static QmlPreviewPlugin *theInstance = nullptr;

QmlPreviewPlugin::~QmlPreviewPlugin()
{
    delete d;
    theInstance = nullptr;
}

QmlPreviewPlugin *QmlPreviewPlugin::instance()
{
    return theInstance;
}

const QmlPreviewRunnerSetting &QmlPreviewPlugin::settings()
{
    return theInstance->d->m_settings;
}

void QmlPreviewPlugin::initialize()
{
    d = new QmlPreviewPluginPrivate(this);
    theInstance = this;

#ifdef WITH_TESTS
    addTestCreator(createQmlPreviewClientTest);
    addTestCreator(createQmlPreviewPluginTest);
#endif
}

QString QmlPreviewPlugin::previewedFile() const
{
    return d->m_previewedFile;
}

void QmlPreviewPlugin::setPreviewedFile(const QString &previewedFile)
{
    if (d->m_previewedFile == previewedFile)
        return;

    d->m_previewedFile = previewedFile;
    emit previewedFileChanged(d->m_previewedFile);
}

QmlPreviewRunControlList QmlPreviewPlugin::runningPreviews() const
{
    return d->m_runningPreviews;
}

void QmlPreviewPlugin::stopAllPreviews()
{
    for (auto &runningPreview : d->m_runningPreviews)
        runningPreview->initiateStop();
}

QmlPreviewFileLoader QmlPreviewPlugin::fileLoader() const
{
    return d->m_settings.fileLoader;
}

QmlPreviewFileClassifier QmlPreviewPlugin::fileClassifier() const
{
    return d->m_settings.fileClassifier;
}

void QmlPreviewPlugin::setFileClassifier(QmlPreviewFileClassifier fileClassifier)
{
    if (d->m_settings.fileClassifier == fileClassifier)
        return;

    d->m_settings.fileClassifier = fileClassifier;
    emit fileClassifierChanged(d->m_settings.fileClassifier);
}

float QmlPreviewPlugin::zoomFactor() const
{
    return d->m_settings.zoomFactor;
}

void QmlPreviewPlugin::setZoomFactor(float zoomFactor)
{
    if (d->m_settings.zoomFactor == zoomFactor)
        return;

    d->m_settings.zoomFactor = zoomFactor;
    emit zoomFactorChanged(d->m_settings.zoomFactor);
}

QmlPreviewFpsHandler QmlPreviewPlugin::fpsHandler() const
{
    return d->m_settings.fpsHandler;
}

void QmlPreviewPlugin::setFpsHandler(QmlPreviewFpsHandler fpsHandler)
{
    if (d->m_settings.fpsHandler == fpsHandler)
        return;

    d->m_settings.fpsHandler = fpsHandler;
    emit fpsHandlerChanged(d->m_settings.fpsHandler);
}

QString QmlPreviewPlugin::localeIsoCode() const
{
    return d->m_localeIsoCode;
}

void QmlPreviewPlugin::setLocaleIsoCode(const QString &localeIsoCode)
{
    if (auto multiLanguageAspect = QmlProjectManager::QmlMultiLanguageAspect::current())
        multiLanguageAspect->setCurrentLocale(localeIsoCode);
    if (d->m_localeIsoCode == localeIsoCode)
        return;

    d->m_localeIsoCode = localeIsoCode;
    emit localeIsoCodeChanged(d->m_localeIsoCode);
}

void QmlPreviewPlugin::setQmlDebugTranslationClientCreator(QmlDebugTranslationClientFactoryFunction creator)
{
    d->m_settings.createDebugTranslationClientMethod = creator;
}

void QmlPreviewPlugin::setRefreshTranslationsFunction(QmlPreviewRefreshTranslationFunction refreshTranslationsFunction)
{
    d->m_settings.refreshTranslationsFunction = refreshTranslationsFunction;
}

void QmlPreviewPlugin::setFileLoader(QmlPreviewFileLoader fileLoader)
{
    if (d->m_settings.fileLoader == fileLoader)
        return;

    d->m_settings.fileLoader = fileLoader;
    emit fileLoaderChanged(d->m_settings.fileLoader);
}

void QmlPreviewPlugin::previewCurrentFile()
{
    const Node *currentNode = ProjectTree::currentNode();
    if (!currentNode || !currentNode->asFileNode()
            || currentNode->asFileNode()->fileType() != FileType::QML)
        return;

    if (runningPreviews().isEmpty())
        QMessageBox::warning(Core::ICore::dialogParent(), Tr::tr("QML Preview Not Running"),
                             Tr::tr("Start the QML Preview for the project before selecting "
                                    "a specific file for preview."));

    const QString file = currentNode->filePath().toUrlishString();
    if (file != d->m_previewedFile)
        setPreviewedFile(file);
    else
        d->checkFile(file);
}

void QmlPreviewPluginPrivate::onEditorChanged(Core::IEditor *editor)
{
    if (m_lastEditor && m_lastEditor->document()) {
        disconnect(m_lastEditor->document(), &Core::IDocument::contentsChanged,
                   this, &QmlPreviewPluginPrivate::setDirty);
        if (m_dirty) {
            m_dirty = false;
            checkEditor();
        }
    }

    m_lastEditor = editor;
    if (m_lastEditor) {
        // Handle new editor
        connect(m_lastEditor->document(), &Core::IDocument::contentsChanged,
                this, &QmlPreviewPluginPrivate::setDirty);
    }
}

void QmlPreviewPluginPrivate::onEditorAboutToClose(Core::IEditor *editor)
{
    if (m_lastEditor != editor)
        return;

    // Oh no our editor is going to be closed
    // get the content first
    Core::IDocument *doc = m_lastEditor->document();
    disconnect(doc, &Core::IDocument::contentsChanged, this, &QmlPreviewPluginPrivate::setDirty);
    if (m_dirty) {
        m_dirty = false;
        checkEditor();
    }
    m_lastEditor = nullptr;
}

void QmlPreviewPluginPrivate::setDirty()
{
    m_dirty = true;
    QTimer::singleShot(1000, this, [this] {
        if (m_dirty && m_lastEditor) {
            m_dirty = false;
            checkEditor();
        }
    });
}

void QmlPreviewPlugin::addPreview(RunControl *preview)
{
    d->attachToEditorManager();
    d->setDirty();
    d->onEditorChanged(Core::EditorManager::currentEditor());

    d->m_runningPreviews.append(preview);
    emit runningPreviewsChanged(d->m_runningPreviews);
}

void QmlPreviewPlugin::removePreview(RunControl *preview)
{
    d->m_runningPreviews.removeOne(preview);
    emit runningPreviewsChanged(d->m_runningPreviews);
    if (d->m_runningPreviews.isEmpty()) {
        if (auto cmd = Core::ActionManager::command("QmlPreview.RunPreview"); cmd && cmd->action())
            cmd->action()->setEnabled(true);
        d->detachFromEditorManager();
    }
}

void QmlPreviewPluginPrivate::attachToEditorManager()
{
    Core::EditorManager *editorManager = Core::EditorManager::instance();
    connect(editorManager, &Core::EditorManager::currentEditorChanged,
            this, &QmlPreviewPluginPrivate::onEditorChanged);
    connect(editorManager, &Core::EditorManager::editorAboutToClose,
            this, &QmlPreviewPluginPrivate::onEditorAboutToClose);
}

void QmlPreviewPluginPrivate::detachFromEditorManager()
{
    Core::EditorManager *editorManager = Core::EditorManager::instance();
    disconnect(editorManager, &Core::EditorManager::currentEditorChanged,
               this, &QmlPreviewPluginPrivate::onEditorChanged);
    disconnect(editorManager, &Core::EditorManager::editorAboutToClose,
               this, &QmlPreviewPluginPrivate::onEditorAboutToClose);
}

void QmlPreviewPluginPrivate::checkEditor()
{
    QmlJS::Dialect::Enum dialect = QmlJS::Dialect::AnyLanguage;
    Core::IDocument *doc = m_lastEditor->document();
    using namespace Utils::Constants;
    const QString mimeType = doc->mimeType();
    if (mimeType == JS_MIMETYPE)
        dialect = QmlJS::Dialect::JavaScript;
    else if (mimeType == JSON_MIMETYPE)
        dialect = QmlJS::Dialect::Json;
    else if (mimeType == QML_MIMETYPE)
        dialect = QmlJS::Dialect::Qml;
//  --- Can we detect those via mime types?
//  else if (mimeType == ???)
//      dialect = QmlJS::Dialect::QmlQtQuick1;
//  else if (mimeType == ???)
//      dialect = QmlJS::Dialect::QmlQtQuick2;
    else if (mimeType == QBS_MIMETYPE)
        dialect = QmlJS::Dialect::QmlQbs;
    else if (mimeType == QMLPROJECT_MIMETYPE)
        dialect = QmlJS::Dialect::QmlProject;
    else if (mimeType == QMLTYPES_MIMETYPE)
        dialect = QmlJS::Dialect::QmlTypeInfo;
    else if (mimeType == QMLUI_MIMETYPE)
        dialect = QmlJS::Dialect::QmlQtQuick2Ui;
    else
        dialect = QmlJS::Dialect::NoLanguage;
    checkDocument(doc->filePath().toUrlishString(), doc->contents(), dialect);
}

void QmlPreviewPluginPrivate::checkFile(const QString &fileName)
{
    if (!m_settings.fileLoader)
        return;

    bool success = false;
    const QByteArray contents = m_settings.fileLoader(fileName, &success);

    if (success) {
        checkDocument(fileName, contents, QmlJS::ModelManagerInterface::guessLanguageOfFile(
                                              Utils::FilePath::fromUserInput(fileName)).dialect());
    }
}

void QmlPreviewPluginPrivate::triggerPreview(const QString &changedFile, const QByteArray &contents)
{
    if (m_previewedFile.isEmpty())
        q->previewCurrentFile();
    else
        emit q->updatePreviews(m_previewedFile, changedFile, contents);
}

static void parse(QPromise<void> &promise, const QString &name, const QByteArray &contents,
                  QmlJS::Dialect::Enum dialect)
{
    if (!QmlJS::Dialect(dialect).isQmlLikeOrJsLanguage())
        return;

    auto qmljsDoc = QmlJS::Document::create(Utils::FilePath::fromString(name), dialect);
    if (promise.isCanceled())
        return;

    qmljsDoc->setSource(QString::fromUtf8(contents));
    if (!qmljsDoc->parse())
        promise.future().cancel();
}

void QmlPreviewPluginPrivate::checkDocument(const QString &name, const QByteArray &contents,
                                            QmlJS::Dialect::Enum dialect)
{
    const auto onParseSetup = [name, contents, dialect](Utils::Async<void> &async) {
        async.setConcurrentCallData(parse, name, contents, dialect);
    };
    const auto onDone = [this, name, contents](DoneWith result) {
        if (result == DoneWith::Success)
            triggerPreview(name, contents);
    };
    m_parseRunner.start({Utils::AsyncTask<void>(onParseSetup)}, {}, onDone);
}

} // namespace QmlPreview
