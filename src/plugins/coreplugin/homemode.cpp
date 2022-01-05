#include "coreconstants.h"
#include "coreicons.h"
#include "homemode.h"
#include "icore.h"
#include "modemanager.h"
#include "minisplitter.h"
#include "navigationwidget.h"
#include "outputpane.h"
#include "rightpane.h"
#include <coreplugin/homemanager/homemanager.h>
#include <coreplugin/homemanager/ieditor.h>

#include <QLatin1String>
#include <QHBoxLayout>
#include <QWidget>
#include <QIcon>

namespace Core {
namespace Internal {

HomeMode::HomeMode() :
    m_splitter(new MiniSplitter),
    m_rightSplitWidgetLayout(new QVBoxLayout)
{
    setObjectName(QLatin1String("HomeMode"));
    setDisplayName(tr("Home"));
    setIcon(Utils::Icon::modeIcon(Icons::MODE_HOME_CLASSIC,
                                  Icons::MODE_HOME_FLAT, Icons::MODE_HOME_FLAT_ACTIVE));
    setPriority(Constants::P_MODE_HOME);
    setId(Constants::MODE_HOME);

    m_rightSplitWidgetLayout->setSpacing(0);
    m_rightSplitWidgetLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *rightSplitWidget = new QWidget;
    rightSplitWidget->setLayout(m_rightSplitWidgetLayout);
    auto editorPlaceHolder = new HomeManagerPlaceHolder;
    m_rightSplitWidgetLayout->insertWidget(0, editorPlaceHolder);

    auto rightPaneSplitter = new MiniSplitter;
    rightPaneSplitter->insertWidget(0, rightSplitWidget);
    rightPaneSplitter->insertWidget(1, new RightPanePlaceHolder(Constants::MODE_HOME));
    rightPaneSplitter->setStretchFactor(0, 1);
    rightPaneSplitter->setStretchFactor(1, 0);

    auto splitter = new MiniSplitter;
    splitter->setOrientation(Qt::Vertical);
    splitter->insertWidget(0, rightPaneSplitter);
    QWidget *outputPane = new OutputPanePlaceHolder(Constants::MODE_HOME, splitter);
    outputPane->setObjectName(QLatin1String("HomeModeOutputPanePlaceHolder"));
    splitter->insertWidget(1, outputPane);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 0);

    m_splitter->insertWidget(0, new NavigationWidgetPlaceHolder(Constants::MODE_HOME, Side::Left));
    m_splitter->insertWidget(1, splitter);
    m_splitter->insertWidget(2, new NavigationWidgetPlaceHolder(Constants::MODE_HOME, Side::Right));
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setStretchFactor(2, 0);

    connect(ModeManager::instance(), &ModeManager::currentModeChanged,
            this, &HomeMode::grabEditorManager);
    m_splitter->setFocusProxy(editorPlaceHolder);

    auto modeContextObject = new IContext(this);
    modeContextObject->setContext(Context(Constants::C_HOME_MANAGER));
    modeContextObject->setWidget(m_splitter);
    ICore::addContextObject(modeContextObject);

    setWidget(m_splitter);
    setContext(Context(Constants::C_HOME_MODE,
                       Constants::C_NAVIGATION_PANE));
}

HomeMode::~HomeMode()
{
    delete m_splitter;
}

void HomeMode::grabEditorManager(Utils::Id mode)
{
    if (mode != id())
        return;

    if (HomeManager::currentEditor())
        HomeManager::currentEditor()->widget()->setFocus();
}

} // namespace Internal
} // namespace Core
