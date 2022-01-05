#include "homewindow.h"

#include "homearea.h"
#include "homemanager_p.h"

#include <aggregation/aggregate.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <coreplugin/minisplitter.h>
#include <utils/qtcassert.h>

#include <QStatusBar>
#include <QVBoxLayout>

const char geometryKey[] = "geometry";
const char splitStateKey[] = "splitstate";

namespace Core {
namespace Internal {

HomeWindow::HomeWindow(QWidget *parent) :
    QWidget(parent)
{
    m_area = new HomeArea;
    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
    layout->addWidget(m_area);
    setFocusProxy(m_area);
    auto statusBar = new QStatusBar;
    layout->addWidget(statusBar);
    auto splitter = new NonResizingSplitter(statusBar);
    splitter->setChildrenCollapsible(false);
    statusBar->addPermanentWidget(splitter, 0);

    splitter->addWidget(new QWidget);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, false); // don't prevent Qt Creator from closing
    resize(QSize(800, 600));

    static int windowId = 0;

    ICore::registerWindow(this,
                          Context(Utils::Id("HomeManager.ExternalWindow.").withSuffix(++windowId),
                                  Constants::C_HOME_MANAGER));

    connect(m_area, &HomeArea::windowTitleNeedsUpdate,
            this, &HomeWindow::updateWindowTitle);
    // editor area can be deleted by editor manager
    connect(m_area, &HomeArea::destroyed, this, [this]() {
        m_area = nullptr;
        deleteLater();
    });
    updateWindowTitle();
}

HomeWindow::~HomeWindow()
{
    if (m_area)
        disconnect(m_area, nullptr, this, nullptr);
}

HomeArea *HomeWindow::editorArea() const
{
    return m_area;
}

QVariantHash HomeWindow::saveState() const
{
    QVariantHash state;
    state.insert(geometryKey, saveGeometry());
    if (QTC_GUARD(m_area)) {
        const QByteArray splitState = m_area->saveState();
        state.insert(splitStateKey, splitState);
    }
    return state;
}

void HomeWindow::restoreState(const QVariantHash &state)
{
    if (state.contains(geometryKey))
        restoreGeometry(state.value(geometryKey).toByteArray());
    if (state.contains(splitStateKey) && m_area)
        m_area->restoreState(state.value(splitStateKey).toByteArray());
}

void HomeWindow::updateWindowTitle()
{
    HomeManagerPrivate::updateWindowTitleForDocument(m_area->currentDocument(), this);
}

} // Internal
} // Core
