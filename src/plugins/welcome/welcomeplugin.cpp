#include "introductionwidget.h"

#include <extensionsystem/iplugin.h>
#include <extensionsystem/pluginmanager.h>

#include <app/app_version.h>

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <coreplugin/iwelcomepage.h>
#include <coreplugin/iwizardfactory.h>
#include <coreplugin/modemanager.h>

#include <utils/algorithm.h>
#include <utils/fileutils.h>
#include <utils/hostosinfo.h>
#include <utils/icon.h>
#include <utils/porting.h>
#include <utils/qtcassert.h>
#include <utils/styledbar.h>
#include <utils/theme/theme.h>
#include <utils/treemodel.h>

#include <QDesktopServices>
#include <QHeaderView>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>

using namespace Core;
using namespace ExtensionSystem;
using namespace Utils;

namespace Welcome {
namespace Internal {

class SideBar;

const int lrPadding = 34;
const char currentPageSettingsKeyC[] = "Welcome2Tab";

static QColor themeColor(Theme::Color role)
{
    return Utils::creatorTheme()->color(role);
}

static QFont sizedFont(int size, const QWidget *widget, bool underline = false)
{
    QFont f = widget->font();
    f.setPixelSize(size);
    f.setUnderline(underline);
    return f;
}

static QPalette lightText()
{
    QPalette pal;
    pal.setColor(QPalette::WindowText, themeColor(Theme::Welcome_LinkColor));
    return pal;
}

static void addWeakVerticalSpacerToLayout(QVBoxLayout *layout, int maximumSize)
{
    auto weakSpacer = new QWidget;
    weakSpacer->setMaximumHeight(maximumSize);
    weakSpacer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
    layout->addWidget(weakSpacer);
    layout->setStretchFactor(weakSpacer, 1);
}

class WelcomeMode : public IMode
{
    Q_OBJECT
public:
    WelcomeMode();
    ~WelcomeMode();

    void initPlugins();

    Q_INVOKABLE bool openDroppedFiles(const QList<QUrl> &urls);

private:
    void addPage(IWelcomePage *page);

    QWidget *m_modeWidget;
    QStackedWidget *m_pageStack;
    SideBar *m_sideBar;
    QList<IWelcomePage *> m_pluginList;
    QList<WelcomePageButton *> m_pageButtons;
    Id m_activePage;
    Id m_defaultPage;
};

class WelcomePlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.milab.Qt.MFDSPlugin" FILE "Welcome.json")

public:
    ~WelcomePlugin() final { delete m_welcomeMode; }

    bool initialize(const QStringList &arguments, QString *) final
    {
        m_welcomeMode = new WelcomeMode;

        auto introAction = new QAction(tr("UI Tour"), this);
        connect(introAction, &QAction::triggered, this, []() {
            auto intro = new IntroductionWidget(ICore::mainWindow());
            intro->show();
        });
        Command *cmd = ActionManager::registerAction(introAction, "Welcome.UITour");
        ActionContainer *mhelp = ActionManager::actionContainer(Core::Constants::M_HELP);
        if (QTC_GUARD(mhelp))
            mhelp->addAction(cmd, Core::Constants::G_HELP_HELP);

        if (!arguments.contains("-notour")) {
            connect(ICore::instance(), &ICore::coreOpened, this, []() {
                IntroductionWidget::askUserAboutIntroduction(ICore::mainWindow(),
                                                             ICore::settings());
            }, Qt::QueuedConnection);
        }

        return true;
    }

    void extensionsInitialized() final
    {
        m_welcomeMode->initPlugins();
        ModeManager::activateMode(m_welcomeMode->id());
    }

    WelcomeMode *m_welcomeMode = nullptr;
};

class IconAndLink : public QWidget
{
public:
    IconAndLink(const QString &iconSource,
                const QString &title,
                const QString &openUrl,
                QWidget *parent)
        : QWidget(parent), m_iconSource(iconSource), m_title(title), m_openUrl(openUrl)
    {
        setAutoFillBackground(true);
        setMinimumHeight(30);
        setToolTip(m_openUrl);

        const QString fileName = QString(":/welcome/images/%1.png").arg(iconSource);
        const Icon icon({{fileName, Theme::Welcome_ForegroundPrimaryColor}}, Icon::Tint);

        m_icon = new QLabel;
        m_icon->setPixmap(icon.pixmap());

        m_label = new QLabel(title);
        m_label->setFont(sizedFont(12, m_label, true));

        auto layout = new QHBoxLayout;
        layout->setContentsMargins(lrPadding, 0, lrPadding, 0);
        layout->addWidget(m_icon);
        layout->addSpacing(2);
        layout->addWidget(m_label);
        layout->addStretch(1);
        setLayout(layout);
    }

    void enterEvent(EnterEvent *) override
    {
        QPalette pal;
        pal.setColor(QPalette::Window, themeColor(Theme::Welcome_HoverColor));
        setPalette(pal);
        m_label->setFont(sizedFont(11, m_label, true));
        update();
    }

    void leaveEvent(QEvent *) override
    {
        QPalette pal;
        pal.setColor(QPalette::Window, themeColor(Theme::Welcome_BackgroundColor));
        setPalette(pal);
        m_label->setFont(sizedFont(11, m_label, false));
        update();
    }

    void mousePressEvent(QMouseEvent *) override
    {
        QDesktopServices::openUrl(m_openUrl);
    }

    QString m_iconSource;
    QString m_title;
    const QString m_openUrl;

    QLabel *m_icon;
    QLabel *m_label;
};

class SideBar : public QWidget
{
    Q_OBJECT
public:
    SideBar(QWidget *parent)
        : QWidget(parent)
    {
        setAutoFillBackground(true);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        setPalette(themeColor(Theme::Welcome_BackgroundColor));

        auto vbox = new QVBoxLayout(this);
        vbox->setSpacing(0);
        vbox->setContentsMargins(0, 27, 0, 0);

        {
            auto l = new QVBoxLayout;
            l->setContentsMargins(lrPadding, 0, lrPadding, 0);
            l->setSpacing(12);

            // MILab LOGO
            auto milabLogoLabel = new QLabel(tr("MILab"), this);
            milabLogoLabel->setFont(sizedFont(18, this));
            QPixmap qp=QPixmap(":/core/images/logo/128/milab_logo.png");
            milabLogoLabel->setPixmap(qp);
            l->addWidget(milabLogoLabel);
            l->addSpacing(27);

            // Get Started Now
            auto getStartedButton = new WelcomePageButton(this);
            getStartedButton->setText(tr("Get Started Now"));
            getStartedButton->setOnClicked([] {
                // QDesktopServices::openUrl(QString("https://scumed.machineilab.org"));
                // New project
                if (!ICore::isNewItemDialogRunning()) {
                    ICore::showNewItemDialog(tr("New Project", "Title of dialog"),
                                             IWizardFactory::allWizardFactories(), QString());
                } else {
                    ICore::raiseWindow(ICore::newItemDialog());
                }
            });
            l->addWidget(getStartedButton);

            // TODO - Jump to Chat (version 2)
            auto jump2ChatButton = new WelcomePageButton(this);
            jump2ChatButton->setText(tr("Chat"));
            jump2ChatButton->setOnClicked([] {
                QDesktopServices::openUrl(QString("https://scumed.machineilab.org"));
            });
            l->addWidget(jump2ChatButton);

            // TODO - Mine, may create a new plugin (e.g.MineWelcomePage)
            // In Mine page, add a way to jump to Genertal Settings
            auto mineButton = new WelcomePageButton(this);
            mineButton->setText(tr("Mine"));
            mineButton->setOnClicked([] {
                QDesktopServices::openUrl(QString("https://scumed.machineilab.org"));
            });
            l->addWidget(mineButton);

            vbox->addItem(l);
        }

        addWeakVerticalSpacerToLayout(vbox, 62);

        {
            auto l = m_pluginButtons = new QVBoxLayout;
            l->setContentsMargins(lrPadding, 0, lrPadding, 0);
            l->setSpacing(19);
            vbox->addItem(l);
        }

        vbox->addStretch(1);

        {
            auto l = new QVBoxLayout;
            l->setContentsMargins(0, 0, 0, 0);
            l->setSpacing(5);
            /* Move this to mode support
            l->addWidget(new IconAndLink("userguide",          tr("User Guide"),
                                         "https://scumed.machineilab.org", this));
            l->addWidget(new IconAndLink("account",            tr("Account"),
                                         "https://scumed.machineilab.org", this));
            l->addWidget(new IconAndLink("community",          tr("Community"),
                                         "https://scumed.machineilab.org", this));
            l->addWidget(new IconAndLink("download",           tr("Get MFDS"),
                                         "https://scumed.machineilab.org", this));*/
            l->addWidget(new IconAndLink("mode_welcome@16x16", tr("MILab"),
                                         "https://scumed.machineilab.org", this));
            vbox->addItem(l);
        }

        addWeakVerticalSpacerToLayout(vbox, vbox->contentsMargins().top());
    }

    QVBoxLayout *m_pluginButtons = nullptr;
};

WelcomeMode::WelcomeMode()
{
    setDisplayName(tr("Welcome"));

    const Icon CLASSIC(":/welcome/images/mode_welcome@24x24.png");
    const Icon FLAT({{":/welcome/images/mode_welcome_mask@24x24.png",
                      Theme::IconsBaseColor}});
    const Icon FLAT_ACTIVE({{":/welcome/images/mode_welcome_mask@24x24.png",
                             Theme::IconsModeWelcomeActiveColor}});
    setIcon(Icon::modeIcon(CLASSIC, FLAT, FLAT_ACTIVE));

    setPriority(Constants::P_MODE_WELCOME);
    setId(Constants::MODE_WELCOME);
    setContextHelp("Manual");
    setContext(Context(Constants::C_WELCOME_MODE));

    QPalette palette = creatorTheme()->palette();
    palette.setColor(QPalette::Window, themeColor(Theme::Welcome_BackgroundColor));

    m_modeWidget = new QWidget;
    m_modeWidget->setPalette(palette);

    m_sideBar = new SideBar(m_modeWidget);
    auto scrollableSideBar = new QScrollArea(m_modeWidget);
    scrollableSideBar->setWidget(m_sideBar);
    scrollableSideBar->setWidgetResizable(true);
    scrollableSideBar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollableSideBar->setFrameShape(QFrame::NoFrame);

    auto divider = new QWidget(m_modeWidget);
    divider->setMaximumWidth(1);
    divider->setMinimumWidth(1);
    divider->setAutoFillBackground(true);
    divider->setPalette(themeColor(Theme::Welcome_DividerColor));

    m_pageStack = new QStackedWidget(m_modeWidget);
    m_pageStack->setObjectName("WelcomeScreenStackedWidget");
    m_pageStack->setAutoFillBackground(true);

    auto hbox = new QHBoxLayout;
    hbox->addWidget(scrollableSideBar);
    hbox->addWidget(divider);
    hbox->addWidget(m_pageStack);
    hbox->setStretchFactor(m_pageStack, 10);

    auto layout = new QVBoxLayout(m_modeWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addItem(hbox);

    setWidget(m_modeWidget);
}

WelcomeMode::~WelcomeMode()
{
    QtcSettings *settings = ICore::settings();
    settings->setValueWithDefault(currentPageSettingsKeyC,
                                  m_activePage.toSetting(),
                                  m_defaultPage.toSetting());
    delete m_modeWidget;
}

void WelcomeMode::initPlugins()
{
    QSettings *settings = ICore::settings();
    m_activePage = Id::fromSetting(settings->value(currentPageSettingsKeyC));

    for (IWelcomePage *page : IWelcomePage::allWelcomePages())
        addPage(page);

    if (!m_pageButtons.isEmpty()) {
        const int welcomeIndex = Utils::indexOf(m_pluginList,
                                                Utils::equal(&IWelcomePage::id,
                                                             Utils::Id("Examples")));
        const int defaultIndex = welcomeIndex >= 0 ? welcomeIndex : 0;
        m_defaultPage = m_pluginList.at(defaultIndex)->id();
        if (!m_activePage.isValid())
            m_pageButtons.at(defaultIndex)->click();
    }
}

bool WelcomeMode::openDroppedFiles(const QList<QUrl> &urls)
{
    const QList<QUrl> localUrls = Utils::filtered(urls, &QUrl::isLocalFile);
    if (!localUrls.isEmpty()) {
        QTimer::singleShot(0, [localUrls]() {
            ICore::openFiles(Utils::transform(localUrls, &QUrl::toLocalFile), ICore::SwitchMode);
        });
        return true;
    }
    return false;
}

void WelcomeMode::addPage(IWelcomePage *page)
{
    int idx;
    int pagePriority = page->priority();
    for (idx = 0; idx != m_pluginList.size(); ++idx) {
        if (m_pluginList.at(idx)->priority() >= pagePriority)
            break;
    }
    auto pageButton = new WelcomePageButton(m_sideBar);
    auto pageId = page->id();
    pageButton->setText(page->title());
    pageButton->setActiveChecker([this, pageId] { return m_activePage == pageId; });

    m_pluginList.insert(idx, page);
    m_pageButtons.insert(idx, pageButton);

    m_sideBar->m_pluginButtons->insertWidget(idx, pageButton);

    QWidget *stackPage = page->createWidget();
    stackPage->setAutoFillBackground(true);
    m_pageStack->insertWidget(idx, stackPage);

    connect(page, &QObject::destroyed, this, [this, page, stackPage, pageButton] {
        m_pluginList.removeOne(page);
        m_pageButtons.removeOne(pageButton);
        delete pageButton;
        delete stackPage;
    });

    auto onClicked = [this, pageId, stackPage] {
        m_activePage = pageId;
        m_pageStack->setCurrentWidget(stackPage);
        for (WelcomePageButton *pageButton : qAsConst(m_pageButtons))
            pageButton->recheckActive();
    };

    pageButton->setOnClicked(onClicked);
    if (pageId == m_activePage)
        onClicked();
}

} // namespace Internal
} // namespace Welcome

#include "welcomeplugin.moc"
