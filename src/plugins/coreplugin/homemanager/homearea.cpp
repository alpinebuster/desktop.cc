#include "homearea.h"

#include "homemanager.h"
#include "ieditor.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/idocument.h>
#include <coreplugin/icore.h>
#include <utils/qtcassert.h>

#include <QApplication>

namespace Core {
namespace Internal {

HomeArea::HomeArea()
{
    m_context = new IContext;
    m_context->setContext(Context(Constants::C_HOME_MANAGER));
    m_context->setWidget(this);
    ICore::addContextObject(m_context);

    setCurrentView(view());
    updateCloseSplitButton();

    connect(qApp, &QApplication::focusChanged,
            this, &HomeArea::focusChanged);
    connect(this, &SplitterOrView::splitStateChanged, this, &HomeArea::updateCloseSplitButton);
}

HomeArea::~HomeArea()
{
    // disconnect
    setCurrentView(nullptr);
    disconnect(qApp, &QApplication::focusChanged,
               this, &HomeArea::focusChanged);

    delete m_context;
}

IDocument *HomeArea::currentDocument() const
{
    return m_currentDocument;
}

void HomeArea::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    // only interesting if the focus moved within the editor area
    if (!focusWidget() || focusWidget() != now)
        return;
    // find the view with focus
    HomeView *current = findFirstView();
    while (current) {
        if (current->focusWidget() && current->focusWidget() == now) {
            setCurrentView(current);
            break;
        }
        current = current->findNextView();
    }
}

void HomeArea::setCurrentView(HomeView *view)
{
    if (view == m_currentView)
        return;
    if (m_currentView) {
        disconnect(m_currentView.data(), &HomeView::currentEditorChanged,
                   this, &HomeArea::updateCurrentEditor);
    }
    m_currentView = view;
    if (m_currentView) {
        connect(m_currentView.data(), &HomeView::currentEditorChanged,
                this, &HomeArea::updateCurrentEditor);
    }
    updateCurrentEditor(m_currentView ? m_currentView->currentEditor() : nullptr);
}

void HomeArea::updateCurrentEditor(IEditor *editor)
{
    IDocument *document = editor ? editor->document() : nullptr;
    if (document == m_currentDocument)
        return;
    if (m_currentDocument) {
        disconnect(m_currentDocument.data(), &IDocument::changed,
                   this, &HomeArea::windowTitleNeedsUpdate);
    }
    m_currentDocument = document;
    if (m_currentDocument) {
        connect(m_currentDocument.data(), &IDocument::changed,
                this, &HomeArea::windowTitleNeedsUpdate);
    }
    emit windowTitleNeedsUpdate();
}

void HomeArea::updateCloseSplitButton()
{
    if (HomeView *v = view())
        v->setCloseSplitEnabled(false);
}

} // Internal
} // Core
