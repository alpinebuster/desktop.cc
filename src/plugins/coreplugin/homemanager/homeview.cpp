#include "homeview.h"

#include "homemanager.h"
#include "homemanager_p.h"
#include "documentmodel.h"
#include "documentmodel_p.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/homemanager/ieditor.h>
#include <coreplugin/editortoolbar.h>
#include <coreplugin/icore.h>
#include <coreplugin/minisplitter.h>
#include <utils/algorithm.h>
#include <utils/infobar.h>
#include <utils/qtcassert.h>
#include <utils/theme/theme.h>
#include <utils/link.h>
#include <utils/utilsicons.h>

#include <QDebug>

#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QStackedWidget>
#include <QToolButton>
#include <QSplitter>
#include <QStackedLayout>

using namespace Core;
using namespace Core::Internal;
using namespace Utils;

HomeView::HomeView(SplitterOrView *parentSplitterOrView, QWidget *parent) :
    QWidget(parent),
    m_parentSplitterOrView(parentSplitterOrView),
    m_toolBar(new EditorToolBar(this)),
    m_container(new QStackedWidget(this)),
    m_infoBarDisplay(new InfoBarDisplay(this)),
    m_statusHLine(new QFrame(this)),
    m_statusWidget(new QFrame(this))
{
    auto tl = new QVBoxLayout(this);
    tl->setSpacing(0);
    tl->setContentsMargins(0, 0, 0, 0);

    {
        connect(m_toolBar, &EditorToolBar::goBackClicked,
                this, &HomeView::goBackInNavigationHistory);
        connect(m_toolBar, &EditorToolBar::goForwardClicked,
                this, &HomeView::goForwardInNavigationHistory);
        connect(m_toolBar, &EditorToolBar::closeClicked, this, &HomeView::closeCurrentEditor);
        connect(m_toolBar, &EditorToolBar::listSelectionActivated,
                this, &HomeView::listSelectionActivated);
        connect(m_toolBar, &EditorToolBar::currentDocumentMoved,
                this, &HomeView::closeCurrentEditor);
        connect(m_toolBar, &EditorToolBar::horizontalSplitClicked,
                this, &HomeView::splitHorizontally);
        connect(m_toolBar, &EditorToolBar::verticalSplitClicked, this, &HomeView::splitVertically);
        connect(m_toolBar, &EditorToolBar::splitNewWindowClicked, this, &HomeView::splitNewWindow);
        connect(m_toolBar, &EditorToolBar::closeSplitClicked, this, &HomeView::closeSplit);
        m_toolBar->setMenuProvider([this](QMenu *menu) { fillListContextMenu(menu); });
        tl->addWidget(m_toolBar);
    }

    m_infoBarDisplay->setTarget(tl, 1);

    tl->addWidget(m_container);

    {
        // Show new mesages on top of the bottom navigation bar
        m_statusHLine->setFrameStyle(QFrame::HLine);

        m_statusWidget->setFrameStyle(QFrame::NoFrame);
        m_statusWidget->setLineWidth(0);
        m_statusWidget->setAutoFillBackground(true);

        auto hbox = new QHBoxLayout(m_statusWidget);
        hbox->setContentsMargins(1, 0, 1, 1);
        m_statusWidgetLabel = new QLabel;
        m_statusWidgetLabel->setContentsMargins(3, 0, 3, 0);
        hbox->addWidget(m_statusWidgetLabel);
        hbox->addStretch(1);

        m_statusWidgetButton = new QToolButton;
        m_statusWidgetButton->setContentsMargins(0, 0, 0, 0);
        hbox->addWidget(m_statusWidgetButton);

        m_statusHLine->setVisible(false);
        m_statusWidget->setVisible(false);
        tl->addWidget(m_statusHLine);
        tl->addWidget(m_statusWidget);
    }

    // for the case of no document selected
    auto empty = new QWidget;
    empty->hide();
    auto emptyLayout = new QGridLayout(empty);
    empty->setLayout(emptyLayout);
    m_emptyViewLabel = new QLabel;
    connect(HomeManagerPrivate::instance(), &HomeManagerPrivate::placeholderTextChanged,
            m_emptyViewLabel, &QLabel::setText);
    m_emptyViewLabel->setText(HomeManagerPrivate::placeholderText());
    emptyLayout->addWidget(m_emptyViewLabel);
    m_container->addWidget(empty);
    m_widgetEditorMap.insert(empty, nullptr);

    const auto dropSupport = new DropSupport(this, [this](QDropEvent *event, DropSupport *) {
        // do not accept move events except from other editor views (i.e. their tool bars)
        // otherwise e.g. item views that support moving items within themselves would
        // also "move" the item into the editor view, i.e. the item would be removed from the
        // item view
        if (!qobject_cast<EditorToolBar*>(event->source()))
            event->setDropAction(Qt::CopyAction);
        if (event->type() == QDropEvent::DragEnter && !DropSupport::isFileDrop(event))
            return false; // do not accept drops without files
        return event->source() != m_toolBar; // do not accept drops on ourselves
    });
    connect(dropSupport, &DropSupport::filesDropped,
            this, &HomeView::openDroppedFiles);

    updateNavigatorActions();
}

HomeView::~HomeView() = default;

SplitterOrView *HomeView::parentSplitterOrView() const
{
    return m_parentSplitterOrView;
}

HomeView *HomeView::findNextView() const
{
    SplitterOrView *current = parentSplitterOrView();
    QTC_ASSERT(current, return nullptr);
    SplitterOrView *parent = current->findParentSplitter();
    while (parent) {
        QSplitter *splitter = parent->splitter();
        QTC_ASSERT(splitter, return nullptr);
        QTC_ASSERT(splitter->count() == 2, return nullptr);
        // is current the first child? then the next view is the first one in current's sibling
        if (splitter->widget(0) == current) {
            auto second = qobject_cast<SplitterOrView *>(splitter->widget(1));
            QTC_ASSERT(second, return nullptr);
            return second->findFirstView();
        }
        // otherwise go up the hierarchy
        current = parent;
        parent = current->findParentSplitter();
    }
    // current has no parent, so we are at the top and there is no "next" view
    return nullptr;
}

HomeView *HomeView::findPreviousView() const
{
    SplitterOrView *current = parentSplitterOrView();
    QTC_ASSERT(current, return nullptr);
    SplitterOrView *parent = current->findParentSplitter();
    while (parent) {
        QSplitter *splitter = parent->splitter();
        QTC_ASSERT(splitter, return nullptr);
        QTC_ASSERT(splitter->count() == 2, return nullptr);
        // is current the last child? then the previous view is the first child in current's sibling
        if (splitter->widget(1) == current) {
            auto first = qobject_cast<SplitterOrView *>(splitter->widget(0));
            QTC_ASSERT(first, return nullptr);
            return first->findFirstView();
        }
        // otherwise go up the hierarchy
        current = parent;
        parent = current->findParentSplitter();
    }
    // current has no parent, so we are at the top and there is no "previous" view
    return nullptr;
}

void HomeView::closeCurrentEditor()
{
    IEditor *editor = currentEditor();
    if (editor)
       HomeManagerPrivate::closeEditorOrDocument(editor);
}

void HomeView::showEditorStatusBar(const QString &id,
                                     const QString &infoText,
                                     const QString &buttonText,
                                     QObject *object, const std::function<void()> &function)
{
    m_statusWidgetId = id;
    m_statusWidgetLabel->setText(infoText);
    m_statusWidgetButton->setText(buttonText);
    m_statusWidgetButton->setToolTip(buttonText);
    m_statusWidgetButton->disconnect();
    if (object && function)
        connect(m_statusWidgetButton, &QToolButton::clicked, object, function);
    m_statusWidget->setVisible(true);
    m_statusHLine->setVisible(true);
    //m_editorForInfoWidget = currentEditor();
}

void HomeView::hideEditorStatusBar(const QString &id)
{
    if (id == m_statusWidgetId) {
        m_statusWidget->setVisible(false);
        m_statusHLine->setVisible(false);
    }
}

void HomeView::setCloseSplitEnabled(bool enable)
{
    m_toolBar->setCloseSplitEnabled(enable);
}

void HomeView::setCloseSplitIcon(const QIcon &icon)
{
    m_toolBar->setCloseSplitIcon(icon);
}

void HomeView::updateEditorHistory(IEditor *editor, QList<EditLocation> &history)
{
    if (!editor)
        return;
    IDocument *document = editor->document();

    if (!document)
        return;

    QByteArray state = editor->saveState();

    EditLocation location;
    location.document = document;
    location.filePath = document->filePath();
    location.id = document->id();
    location.state = QVariant(state);

    for (int i = 0; i < history.size(); ++i) {
        const EditLocation &item = history.at(i);
        if (item.document == document
                || (!item.document && !DocumentModel::indexOfFilePath(item.filePath))) {
            history.removeAt(i--);
        }
    }
    history.prepend(location);
}

void HomeView::paintEvent(QPaintEvent *)
{
    HomeView *editorView = HomeManagerPrivate::currentEditorView();
    if (editorView != this)
        return;

    if (m_container->currentIndex() != 0) // so a document is selected
        return;

    // Discreet indication where an editor would be if there is none
    QPainter painter(this);

    QRect rect = m_container->geometry();
    if (creatorTheme()->flag(Theme::FlatToolBars)) {
        painter.fillRect(rect, creatorTheme()->color(Theme::EditorPlaceholderColor));
    } else {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::NoPen);
        painter.setBrush(creatorTheme()->color(Theme::EditorPlaceholderColor));
        const int r = 3;
        painter.drawRoundedRect(rect.adjusted(r , r, -r, -r), r * 2, r * 2);
    }
}

void HomeView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton)
        return;
    setFocus(Qt::MouseFocusReason);
}

void HomeView::focusInEvent(QFocusEvent *)
{
    HomeManagerPrivate::setCurrentView(this);
}

void HomeView::addEditor(IEditor *editor)
{
    if (m_editors.contains(editor))
        return;

    m_editors.append(editor);

    m_container->addWidget(editor->widget());
    m_widgetEditorMap.insert(editor->widget(), editor);
    m_toolBar->addEditor(editor);

    if (editor == currentEditor())
        setCurrentEditor(editor);
}

bool HomeView::hasEditor(IEditor *editor) const
{
    return m_editors.contains(editor);
}

void HomeView::removeEditor(IEditor *editor)
{
    QTC_ASSERT(editor, return);
    if (!m_editors.contains(editor))
        return;

    const int index = m_container->indexOf(editor->widget());
    QTC_ASSERT((index != -1), return);
    bool wasCurrent = (index == m_container->currentIndex());
    m_editors.removeAll(editor);

    m_container->removeWidget(editor->widget());
    m_widgetEditorMap.remove(editor->widget());
    editor->widget()->setParent(nullptr);
    m_toolBar->removeToolbarForEditor(editor);

    if (wasCurrent)
        setCurrentEditor(!m_editors.isEmpty() ? m_editors.last() : nullptr);
}

IEditor *HomeView::currentEditor() const
{
    if (!m_editors.isEmpty())
        return m_widgetEditorMap.value(m_container->currentWidget());
    return nullptr;
}

void HomeView::listSelectionActivated(int index)
{
    HomeManagerPrivate::activateEditorForEntry(this, DocumentModel::entryAtRow(index));
}

void HomeView::fillListContextMenu(QMenu *menu) const
{
    IEditor *editor = currentEditor();
    DocumentModel::Entry *entry = editor ? DocumentModel::entryForDocument(editor->document())
                                         : nullptr;
    HomeManager::addSaveAndCloseEditorActions(menu, entry, editor);
    menu->addSeparator();
    HomeManager::addNativeDirAndOpenWithActions(menu, entry);
}

void HomeView::splitHorizontally()
{
    if (m_parentSplitterOrView)
        m_parentSplitterOrView->split(Qt::Vertical);
    HomeManagerPrivate::updateActions();
}

void HomeView::splitVertically()
{
    if (m_parentSplitterOrView)
        m_parentSplitterOrView->split(Qt::Horizontal);
    HomeManagerPrivate::updateActions();
}

void HomeView::splitNewWindow()
{
    HomeManagerPrivate::splitNewWindow(this);
}

void HomeView::closeSplit()
{
    HomeManagerPrivate::closeView(this);
    HomeManagerPrivate::updateActions();
}

void HomeView::openDroppedFiles(const QList<DropSupport::FileSpec> &files)
{
    bool first = true;
    auto specToLink = [](const DropSupport::FileSpec &spec) {
        return Utils::Link(FilePath::fromString(spec.filePath), spec.line, spec.column);
    };
    auto openEntry = [&](const DropSupport::FileSpec &spec) {
        if (first) {
            first = false;
            HomeManagerPrivate::openEditorAt(this, specToLink(spec));
        } else if (spec.column != -1 || spec.line != -1) {
            HomeManagerPrivate::openEditorAt(this,
                                               specToLink(spec),
                                               Id(),
                                               HomeManager::DoNotChangeCurrentEditor
                                                   | HomeManager::DoNotMakeVisible);
        } else {
            auto *factory = IEditorFactory::preferredEditorFactories(spec.filePath).value(0);
            DocumentModelPrivate::addSuspendedDocument(spec.filePath,
                                                       {},
                                                       factory ? factory->id() : Id());
        }
    };
    Utils::reverseForeach(files, openEntry);
}

void HomeView::setParentSplitterOrView(SplitterOrView *splitterOrView)
{
    m_parentSplitterOrView = splitterOrView;
}

void HomeView::setCurrentEditor(IEditor *editor)
{
    if (!editor || m_container->indexOf(editor->widget()) == -1) {
        QTC_CHECK(!editor);
        m_toolBar->setCurrentEditor(nullptr);
        m_infoBarDisplay->setInfoBar(nullptr);
        m_container->setCurrentIndex(0);
        emit currentEditorChanged(nullptr);
        return;
    }

    m_editors.removeAll(editor);
    m_editors.append(editor);

    const int idx = m_container->indexOf(editor->widget());
    QTC_ASSERT(idx >= 0, return);
    m_container->setCurrentIndex(idx);
    m_toolBar->setCurrentEditor(editor);

    updateEditorHistory(editor);

    m_infoBarDisplay->setInfoBar(editor->document()->infoBar());
    emit currentEditorChanged(editor);
}

int HomeView::editorCount() const
{
    return m_editors.size();
}

QList<IEditor *> HomeView::editors() const
{
    return m_editors;
}

IEditor *HomeView::editorForDocument(const IDocument *document) const
{
    foreach (IEditor *editor, m_editors)
        if (editor->document() == document)
            return editor;
    return nullptr;
}

void HomeView::updateEditorHistory(IEditor *editor)
{
    updateEditorHistory(editor, m_editorHistory);
}

void HomeView::addCurrentPositionToNavigationHistory(const QByteArray &saveState)
{
    IEditor *editor = currentEditor();
    if (!editor)
        return;
    IDocument *document = editor->document();

    if (!document)
        return;

    QByteArray state;
    if (saveState.isNull())
        state = editor->saveState();
    else
        state = saveState;

    EditLocation location;
    location.document = document;
    location.filePath = document->filePath();
    location.id = document->id();
    location.state = QVariant(state);
    m_currentNavigationHistoryPosition = qMin(m_currentNavigationHistoryPosition, m_navigationHistory.size()); // paranoia
    m_navigationHistory.insert(m_currentNavigationHistoryPosition, location);
    ++m_currentNavigationHistoryPosition;

    while (m_navigationHistory.size() >= 30) {
        if (m_currentNavigationHistoryPosition > 15) {
            m_navigationHistory.removeFirst();
            --m_currentNavigationHistoryPosition;
        } else {
            m_navigationHistory.removeLast();
        }
    }
    updateNavigatorActions();
}

void HomeView::cutForwardNavigationHistory()
{
    while (m_currentNavigationHistoryPosition < m_navigationHistory.size() - 1)
        m_navigationHistory.removeLast();
}

void HomeView::updateNavigatorActions()
{
    m_toolBar->setCanGoBack(canGoBack());
    m_toolBar->setCanGoForward(canGoForward());
}

void HomeView::copyNavigationHistoryFrom(HomeView* other)
{
    if (!other)
        return;
    m_currentNavigationHistoryPosition = other->m_currentNavigationHistoryPosition;
    m_navigationHistory = other->m_navigationHistory;
    m_editorHistory = other->m_editorHistory;
    updateNavigatorActions();
}

void HomeView::updateCurrentPositionInNavigationHistory()
{
    IEditor *editor = currentEditor();
    if (!editor || !editor->document())
        return;

    IDocument *document = editor->document();
    EditLocation *location;
    if (m_currentNavigationHistoryPosition < m_navigationHistory.size()) {
        location = &m_navigationHistory[m_currentNavigationHistoryPosition];
    } else {
        m_navigationHistory.append(EditLocation());
        location = &m_navigationHistory[m_navigationHistory.size()-1];
    }
    location->document = document;
    location->filePath = document->filePath();
    location->id = document->id();
    location->state = QVariant(editor->saveState());
}

static bool fileNameWasRemoved(const FilePath &filePath)
{
    return !filePath.isEmpty() && !filePath.exists();
}

void HomeView::goBackInNavigationHistory()
{
    updateCurrentPositionInNavigationHistory();
    while (m_currentNavigationHistoryPosition > 0) {
        --m_currentNavigationHistoryPosition;
        EditLocation location = m_navigationHistory.at(m_currentNavigationHistoryPosition);
        IEditor *editor = nullptr;
        if (location.document) {
            editor = HomeManagerPrivate::activateEditorForDocument(this, location.document,
                                        HomeManager::IgnoreNavigationHistory);
        }
        if (!editor) {
            if (fileNameWasRemoved(location.filePath)) {
                m_navigationHistory.removeAt(m_currentNavigationHistoryPosition);
                continue;
            }
            editor = HomeManagerPrivate::openEditor(this, location.filePath, location.id,
                                    HomeManager::IgnoreNavigationHistory);
            if (!editor) {
                m_navigationHistory.removeAt(m_currentNavigationHistoryPosition);
                continue;
            }
        }
        editor->restoreState(location.state.toByteArray());
        break;
    }
    updateNavigatorActions();
}

void HomeView::goForwardInNavigationHistory()
{
    updateCurrentPositionInNavigationHistory();
    if (m_currentNavigationHistoryPosition >= m_navigationHistory.size()-1)
        return;
    ++m_currentNavigationHistoryPosition;
    while (m_currentNavigationHistoryPosition < m_navigationHistory.size()) {
        IEditor *editor = nullptr;
        EditLocation location = m_navigationHistory.at(m_currentNavigationHistoryPosition);
        if (location.document) {
            editor = HomeManagerPrivate::activateEditorForDocument(this, location.document,
                                                                     HomeManager::IgnoreNavigationHistory);
        }
        if (!editor) {
            if (fileNameWasRemoved(location.filePath)) {
                m_navigationHistory.removeAt(m_currentNavigationHistoryPosition);
                continue;
            }
            editor = HomeManagerPrivate::openEditor(this, location.filePath, location.id,
                                                      HomeManager::IgnoreNavigationHistory);
            if (!editor) {
                m_navigationHistory.removeAt(m_currentNavigationHistoryPosition);
                continue;
            }
        }
        editor->restoreState(location.state.toByteArray());
        break;
    }
    if (m_currentNavigationHistoryPosition >= m_navigationHistory.size())
        m_currentNavigationHistoryPosition = qMax<int>(m_navigationHistory.size() - 1, 0);
    updateNavigatorActions();
}

void HomeView::goToEditLocation(const EditLocation &location)
{
    IEditor *editor = nullptr;

    if (location.document) {
        editor = HomeManagerPrivate::activateEditorForDocument(this, location.document,
                                                                 HomeManager::IgnoreNavigationHistory);
    }

    if (!editor) {
        if (fileNameWasRemoved(location.filePath))
            return;

        editor = HomeManagerPrivate::openEditor(this, location.filePath, location.id,
                                                  HomeManager::IgnoreNavigationHistory);
    }

    if (editor) {
        editor->restoreState(location.state.toByteArray());
    }
}


SplitterOrView::SplitterOrView(IEditor *editor)
{
    m_layout = new QStackedLayout(this);
    m_layout->setSizeConstraint(QLayout::SetNoConstraint);
    m_view = new HomeView(this);
    if (editor)
        m_view->addEditor(editor);
    m_splitter = nullptr;
    m_layout->addWidget(m_view);
}

SplitterOrView::SplitterOrView(HomeView *view)
{
    Q_ASSERT(view);
    m_layout = new QStackedLayout(this);
    m_layout->setSizeConstraint(QLayout::SetNoConstraint);
    m_view = view;
    m_view->setParentSplitterOrView(this);
    m_splitter = nullptr;
    m_layout->addWidget(m_view);
}

SplitterOrView::~SplitterOrView()
{
    delete m_layout;
    m_layout = nullptr;
    if (m_view)
        HomeManagerPrivate::deleteEditors(HomeManagerPrivate::emptyView(m_view));
    delete m_view;
    m_view = nullptr;
    delete m_splitter;
    m_splitter = nullptr;
}

HomeView *SplitterOrView::findFirstView()
{
    if (m_splitter) {
        for (int i = 0; i < m_splitter->count(); ++i) {
            if (auto splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i)))
                if (HomeView *result = splitterOrView->findFirstView())
                    return result;
        }
        return nullptr;
    }
    return m_view;
}

HomeView *SplitterOrView::findLastView()
{
    if (m_splitter) {
        for (int i = m_splitter->count() - 1; 0 < i; --i) {
            if (auto splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i)))
                if (HomeView *result = splitterOrView->findLastView())
                    return result;
        }
        return nullptr;
    }
    return m_view;
}

SplitterOrView *SplitterOrView::findParentSplitter() const
{
    QWidget *w = parentWidget();
    while (w) {
        if (auto splitter = qobject_cast<SplitterOrView *>(w)) {
            QTC_CHECK(splitter->splitter());
            return splitter;
        }
        w = w->parentWidget();
    }
    return nullptr;
}

QSize SplitterOrView::minimumSizeHint() const
{
    if (m_splitter)
        return m_splitter->minimumSizeHint();
    return QSize(64, 64);
}

QSplitter *SplitterOrView::takeSplitter()
{
    QSplitter *oldSplitter = m_splitter;
    if (m_splitter)
        m_layout->removeWidget(m_splitter);
    m_splitter = nullptr;
    return oldSplitter;
}

HomeView *SplitterOrView::takeView()
{
    HomeView *oldView = m_view;
    if (m_view) {
        // the focus update that is triggered by removing should already have 0 parent
        // so we do that first
        m_view->setParentSplitterOrView(nullptr);
        m_layout->removeWidget(m_view);
    }
    m_view = nullptr;
    return oldView;
}

void SplitterOrView::split(Qt::Orientation orientation, bool activateView)
{
    Q_ASSERT(m_view && m_splitter == nullptr);
    m_splitter = new MiniSplitter(this);
    m_splitter->setOrientation(orientation);
    m_layout->addWidget(m_splitter);
    m_layout->removeWidget(m_view);
    HomeView *editorView = m_view;
    editorView->setCloseSplitEnabled(true); // might have been disabled for root view
    m_view = nullptr;
    IEditor *e = editorView->currentEditor();
    const QByteArray state = e ? e->saveState() : QByteArray();

    SplitterOrView *view = nullptr;
    SplitterOrView *otherView = nullptr;
    IEditor *duplicate = e && e->duplicateSupported() ? HomeManagerPrivate::duplicateEditor(e) : nullptr;
    m_splitter->addWidget((view = new SplitterOrView(duplicate)));
    m_splitter->addWidget((otherView = new SplitterOrView(editorView)));

    m_layout->setCurrentWidget(m_splitter);

    view->view()->copyNavigationHistoryFrom(editorView);
    view->view()->setCurrentEditor(duplicate);

    if (orientation == Qt::Horizontal) {
        view->view()->setCloseSplitIcon(Utils::Icons::CLOSE_SPLIT_LEFT.icon());
        otherView->view()->setCloseSplitIcon(Utils::Icons::CLOSE_SPLIT_RIGHT.icon());
    } else {
        view->view()->setCloseSplitIcon(Utils::Icons::CLOSE_SPLIT_TOP.icon());
        otherView->view()->setCloseSplitIcon(Utils::Icons::CLOSE_SPLIT_BOTTOM.icon());
    }

    // restore old state, possibly adapted to the new layout (the editors can e.g. make sure that
    // a previously visible text cursor stays visible)
    if (duplicate)
        duplicate->restoreState(state);
    if (e)
        e->restoreState(state);

    if (activateView)
        HomeManagerPrivate::activateView(otherView->view());
    emit splitStateChanged();
}

void SplitterOrView::unsplitAll()
{
    QTC_ASSERT(m_splitter, return);
    // avoid focus changes while unsplitting is in progress
    bool hadFocus = false;
    if (QWidget *w = focusWidget()) {
        if (w->hasFocus()) {
            w->clearFocus();
            hadFocus = true;
        }
    }

    HomeView *currentView = HomeManagerPrivate::currentEditorView();
    if (currentView) {
        currentView->parentSplitterOrView()->takeView();
        currentView->setParentSplitterOrView(this);
    } else {
        currentView = new HomeView(this);
    }
    m_splitter->hide();
    m_layout->removeWidget(m_splitter); // workaround Qt bug
    const QList<IEditor *> editorsToDelete = unsplitAll_helper();
    m_view = currentView;
    m_layout->addWidget(m_view);
    delete m_splitter;
    m_splitter = nullptr;

    // restore some focus
    if (hadFocus) {
        if (IEditor *editor = m_view->currentEditor())
            editor->widget()->setFocus();
        else
            m_view->setFocus();
    }
    HomeManagerPrivate::deleteEditors(editorsToDelete);
    emit splitStateChanged();
}

/*!
    Recursively empties all views.
    Returns the editors to delete with EditorManagerPrivate::deleteEditors.
    \internal
*/
const QList<IEditor *> SplitterOrView::unsplitAll_helper()
{
    if (m_view)
        return HomeManagerPrivate::emptyView(m_view);
    QList<IEditor *> editorsToDelete;
    if (m_splitter) {
        for (int i = 0; i < m_splitter->count(); ++i) {
            if (auto splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i)))
                editorsToDelete.append(splitterOrView->unsplitAll_helper());
        }
    }
    return editorsToDelete;
}

void SplitterOrView::unsplit()
{
    if (!m_splitter)
        return;

    Q_ASSERT(m_splitter->count() == 1);
    auto childSplitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(0));
    QSplitter *oldSplitter = m_splitter;
    m_splitter = nullptr;
    QList<IEditor *> editorsToDelete;
    if (childSplitterOrView->isSplitter()) {
        Q_ASSERT(childSplitterOrView->view() == nullptr);
        m_splitter = childSplitterOrView->takeSplitter();
        m_layout->addWidget(m_splitter);
        m_layout->setCurrentWidget(m_splitter);
    } else {
        HomeView *childView = childSplitterOrView->view();
        Q_ASSERT(childView);
        if (m_view) {
            m_view->copyNavigationHistoryFrom(childView);
            if (IEditor *e = childView->currentEditor()) {
                childView->removeEditor(e);
                m_view->addEditor(e);
                m_view->setCurrentEditor(e);
            }
            editorsToDelete = HomeManagerPrivate::emptyView(childView);
        } else {
            m_view = childSplitterOrView->takeView();
            m_view->setParentSplitterOrView(this);
            m_layout->addWidget(m_view);
            auto parentSplitter = qobject_cast<QSplitter *>(parentWidget());
            if (parentSplitter) { // not the toplevel splitterOrView
                if (parentSplitter->orientation() == Qt::Horizontal)
                    m_view->setCloseSplitIcon(parentSplitter->widget(0) == this ?
                                                  Utils::Icons::CLOSE_SPLIT_LEFT.icon()
                                                : Utils::Icons::CLOSE_SPLIT_RIGHT.icon());
                else
                    m_view->setCloseSplitIcon(parentSplitter->widget(0) == this ?
                                                  Utils::Icons::CLOSE_SPLIT_TOP.icon()
                                                : Utils::Icons::CLOSE_SPLIT_BOTTOM.icon());
            }
        }
        m_layout->setCurrentWidget(m_view);
    }
    delete oldSplitter;
    if (HomeView *newCurrent = findFirstView())
        HomeManagerPrivate::activateView(newCurrent);
    else
        HomeManagerPrivate::setCurrentView(nullptr);
    HomeManagerPrivate::deleteEditors(editorsToDelete);
    emit splitStateChanged();
}


QByteArray SplitterOrView::saveState() const
{
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);

    if (m_splitter) {
        stream << QByteArray("splitter")
                << (qint32)m_splitter->orientation()
                << m_splitter->saveState()
                << static_cast<SplitterOrView*>(m_splitter->widget(0))->saveState()
                << static_cast<SplitterOrView*>(m_splitter->widget(1))->saveState();
    } else {
        IEditor* e = editor();

        // don't save state of temporary or ad-hoc editors
        if (e && (e->document()->isTemporary() || e->document()->filePath().isEmpty())) {
            // look for another editor that is more suited
            e = nullptr;
            foreach (IEditor *otherEditor, editors()) {
                if (!otherEditor->document()->isTemporary() && !otherEditor->document()->filePath().isEmpty()) {
                    e = otherEditor;
                    break;
                }
            }
        }

        if (!e) {
            stream << QByteArray("empty");
        } else if (e == HomeManager::currentEditor()) {
            stream << QByteArray("currenteditor")
                   << e->document()->filePath().toString()
                   << e->document()->id().toString()
                   << e->saveState();
        } else {
            stream << QByteArray("editor")
                   << e->document()->filePath().toString()
                   << e->document()->id().toString()
                   << e->saveState();
        }
    }
    return bytes;
}

void SplitterOrView::restoreState(const QByteArray &state)
{
    QDataStream stream(state);
    QByteArray mode;
    stream >> mode;
    if (mode == "splitter") {
        qint32 orientation;
        QByteArray splitter, first, second;
        stream >> orientation >> splitter >> first >> second;
        split((Qt::Orientation) orientation, false);
        m_splitter->restoreState(splitter);
        static_cast<SplitterOrView*>(m_splitter->widget(0))->restoreState(first);
        static_cast<SplitterOrView*>(m_splitter->widget(1))->restoreState(second);
    } else if (mode == "editor" || mode == "currenteditor") {
        QString fileName;
        QString id;
        QByteArray editorState;
        stream >> fileName >> id >> editorState;
        if (!QFile::exists(fileName))
            return;
        IEditor *e = HomeManagerPrivate::openEditor(view(), FilePath::fromString(fileName), Id::fromString(id),
                                                      HomeManager::IgnoreNavigationHistory
                                                      | HomeManager::DoNotChangeCurrentEditor);

        if (!e) {
            DocumentModel::Entry *entry = DocumentModelPrivate::firstSuspendedEntry();
            if (entry) {
                HomeManagerPrivate::activateEditorForEntry(view(), entry,
                    HomeManager::IgnoreNavigationHistory | HomeManager::DoNotChangeCurrentEditor);
            }
        }

        if (e) {
            e->restoreState(editorState);
            if (mode == "currenteditor")
                HomeManagerPrivate::setCurrentEditor(e);
        }
    }
}
