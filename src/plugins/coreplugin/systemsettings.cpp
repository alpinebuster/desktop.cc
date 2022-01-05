#include "systemsettings.h"
#include "coreconstants.h"
#include "coreplugin.h"
#include "homemanager/homemanager_p.h"
#include "dialogs/restartdialog.h"
#include "fileutils.h"
#include "icore.h"
#include "mainwindow.h"

#include <app/app_version.h>
#include <utils/checkablemessagebox.h>
#include <utils/consoleprocess.h>
#include <utils/environment.h>
#include <utils/environmentdialog.h>
#include <utils/hostosinfo.h>
#include <utils/unixutils.h>

#include <QCoreApplication>
#include <QMessageBox>
#include <QSettings>

#include "ui_systemsettings.h"

using namespace Utils;

namespace Core {
namespace Internal {

#ifdef ENABLE_CRASHPAD
const char crashReportingEnabledKey[] = "CrashReportingEnabled";
const char showCrashButtonKey[] = "ShowCrashButton";

// TODO: move to somewhere in Utils
static QString formatSize(qint64 size)
{
    QStringList units {QObject::tr("Bytes"), QObject::tr("KB"), QObject::tr("MB"),
                       QObject::tr("GB"), QObject::tr("TB")};
    double outputSize = size;
    int i;
    for (i = 0; i < units.size() - 1; ++i) {
        if (outputSize < 1024)
            break;
        outputSize /= 1024;
    }
    return i == 0 ? QString("%0 %1").arg(outputSize).arg(units[i]) // Bytes
                  : QString("%0 %1").arg(outputSize, 0, 'f', 2).arg(units[i]); // KB, MB, GB, TB
}
#endif // ENABLE_CRASHPAD

class SystemSettingsWidget : public IOptionsPageWidget
{
    Q_DECLARE_TR_FUNCTIONS(Core::Internal::SystemSettingsWidget)

public:
    SystemSettingsWidget()
    {
        m_ui.setupUi(this);
        m_ui.reloadBehavior->setCurrentIndex(HomeManager::reloadSetting());
        m_ui.autoSaveCheckBox->setChecked(HomeManagerPrivate::autoSaveEnabled());
        m_ui.autoSaveCheckBox->setToolTip(tr("Automatically creates temporary copies of "
                                                "modified files. If %1 is restarted after "
                                                "a crash or power failure, it asks whether to "
                                                "recover the auto-saved content.")
                                             .arg(Constants::IDE_DISPLAY_NAME));
        m_ui.autoSaveInterval->setValue(HomeManagerPrivate::autoSaveInterval());
        m_ui.autoSuspendCheckBox->setChecked(HomeManagerPrivate::autoSuspendEnabled());
        m_ui.autoSuspendMinDocumentCount->setValue(HomeManagerPrivate::autoSuspendMinDocumentCount());
        m_ui.warnBeforeOpeningBigFiles->setChecked(
                    HomeManagerPrivate::warnBeforeOpeningBigFilesEnabled());
        m_ui.bigFilesLimitSpinBox->setValue(HomeManagerPrivate::bigFileSizeLimit());
        m_ui.maxRecentFilesSpinBox->setMinimum(1);
        m_ui.maxRecentFilesSpinBox->setMaximum(99);
        m_ui.maxRecentFilesSpinBox->setValue(HomeManagerPrivate::maxRecentFiles());
#ifdef ENABLE_CRASHPAD
        if (ICore::settings()->value(showCrashButtonKey).toBool()) {
            auto crashButton = new QPushButton("CRASH!!!");
            crashButton->show();
            connect(crashButton, &QPushButton::clicked, []() {
                // do a real crash
                volatile int* a = reinterpret_cast<volatile int *>(NULL); *a = 1;
            });
        }

        m_ui.enableCrashReportingCheckBox->setChecked(
                    ICore::settings()->value(crashReportingEnabledKey).toBool());
        connect(m_ui.helpCrashReportingButton, &QAbstractButton::clicked, this, [this] {
            showHelpDialog(tr("Crash Reporting"), CorePlugin::msgCrashpadInformation());
        });
        connect(m_ui.enableCrashReportingCheckBox,
                QOverload<int>::of(&QCheckBox::stateChanged), this, [this] {
            const QString restartText = tr("The change will take effect after restart.");
            Core::RestartDialog restartDialog(Core::ICore::dialogParent(), restartText);
            restartDialog.exec();
            if (restartDialog.result() == QDialog::Accepted)
                apply();
        });

        updateClearCrashWidgets();
        connect(m_ui.clearCrashReportsButton, &QPushButton::clicked, this, [&] {
            QDir crashReportsDir = ICore::crashReportsPath().toDir();
            crashReportsDir.setFilter(QDir::Files);
            const QStringList crashFiles = crashReportsDir.entryList();
            for (QString file : crashFiles)
                crashReportsDir.remove(file);
            updateClearCrashWidgets();
        });
#else
        m_ui.enableCrashReportingCheckBox->setVisible(false);
        m_ui.helpCrashReportingButton->setVisible(false);
        m_ui.clearCrashReportsButton->setVisible(false);
        m_ui.crashReportsSizeText->setVisible(false);
#endif

        m_ui.askBeforeExitCheckBox->setChecked(
                    static_cast<Core::Internal::MainWindow *>(ICore::mainWindow())->askConfirmationBeforeExit());

        if (HostOsInfo::isMacHost()) {
            Qt::CaseSensitivity defaultSensitivity
                    = OsSpecificAspects::fileNameCaseSensitivity(HostOsInfo::hostOs());
            if (defaultSensitivity == Qt::CaseSensitive) {
                m_ui.fileSystemCaseSensitivityChooser->addItem(tr("Case Sensitive (Default)"),
                                                                  Qt::CaseSensitive);
            } else {
                m_ui.fileSystemCaseSensitivityChooser->addItem(tr("Case Sensitive"),
                                                                  Qt::CaseSensitive);
            }
            if (defaultSensitivity == Qt::CaseInsensitive) {
                m_ui.fileSystemCaseSensitivityChooser->addItem(tr("Case Insensitive (Default)"),
                                                                  Qt::CaseInsensitive);
            } else {
                m_ui.fileSystemCaseSensitivityChooser->addItem(tr("Case Insensitive"),
                                                                  Qt::CaseInsensitive);
            }
            if (HostOsInfo::fileNameCaseSensitivity() == Qt::CaseSensitive)
                m_ui.fileSystemCaseSensitivityChooser->setCurrentIndex(0);
            else
                m_ui.fileSystemCaseSensitivityChooser->setCurrentIndex(1);
        } else {
            m_ui.fileSystemCaseSensitivityLabel->hide();
            m_ui.fileSystemCaseSensitivityWidget->hide();
        }

        m_environmentChanges = CorePlugin::environmentChanges();
    }

private:
    void apply() final;
    void showHelpForFileBrowser();
    void updateClearCrashWidgets();
    void showHelpDialog(const QString &title, const QString &helpText);
    Ui::SystemSettings m_ui;
    QPointer<QMessageBox> m_dialog;
    EnvironmentItems m_environmentChanges;
};

void SystemSettingsWidget::apply()
{
    HomeManager::setReloadSetting(IDocument::ReloadSetting(m_ui.reloadBehavior->currentIndex()));
    HomeManagerPrivate::setAutoSaveEnabled(m_ui.autoSaveCheckBox->isChecked());
    HomeManagerPrivate::setAutoSaveInterval(m_ui.autoSaveInterval->value());
    HomeManagerPrivate::setAutoSuspendEnabled(m_ui.autoSuspendCheckBox->isChecked());
    HomeManagerPrivate::setAutoSuspendMinDocumentCount(m_ui.autoSuspendMinDocumentCount->value());
    HomeManagerPrivate::setWarnBeforeOpeningBigFilesEnabled(
                m_ui.warnBeforeOpeningBigFiles->isChecked());
    HomeManagerPrivate::setBigFileSizeLimit(m_ui.bigFilesLimitSpinBox->value());
    HomeManagerPrivate::setMaxRecentFiles(m_ui.maxRecentFilesSpinBox->value());
#ifdef ENABLE_CRASHPAD
    ICore::settings()->setValue(crashReportingEnabledKey,
                                m_ui.enableCrashReportingCheckBox->isChecked());
#endif

    static_cast<Core::Internal::MainWindow *>(ICore::mainWindow())->setAskConfirmationBeforeExit(
                m_ui.askBeforeExitCheckBox->isChecked());

    if (HostOsInfo::isMacHost()) {
        Qt::CaseSensitivity defaultSensitivity
                = OsSpecificAspects::fileNameCaseSensitivity(HostOsInfo::hostOs());
        Qt::CaseSensitivity selectedSensitivity = Qt::CaseSensitivity(
                m_ui.fileSystemCaseSensitivityChooser->currentData().toInt());
        if (defaultSensitivity == selectedSensitivity)
            HostOsInfo::unsetOverrideFileNameCaseSensitivity();
        else
            HostOsInfo::setOverrideFileNameCaseSensitivity(selectedSensitivity);
    }

    CorePlugin::setEnvironmentChanges(m_environmentChanges);
}

void SystemSettingsWidget::showHelpDialog(const QString &title, const QString &helpText)
{
    if (m_dialog) {
        if (m_dialog->windowTitle() != title)
            m_dialog->setText(helpText);

        if (m_dialog->text() != helpText)
            m_dialog->setText(helpText);

        m_dialog->show();
        ICore::raiseWindow(m_dialog);
        return;
    }
    auto mb = new QMessageBox(QMessageBox::Information, title, helpText, QMessageBox::Close, this);
    mb->setWindowModality(Qt::NonModal);
    m_dialog = mb;
    mb->show();
}

#ifdef ENABLE_CRASHPAD
void SystemSettingsWidget::updateClearCrashWidgets()
{
    QDir crashReportsDir(ICore::crashReportsPath().toDir());
    crashReportsDir.setFilter(QDir::Files);
    qint64 size = 0;
    const QStringList crashFiles = crashReportsDir.entryList();
    for (QString file : crashFiles)
        size += QFileInfo(crashReportsDir, file).size();

    m_ui.clearCrashReportsButton->setEnabled(!crashFiles.isEmpty());
    m_ui.crashReportsSizeText->setText(formatSize(size));
}
#endif

void SystemSettingsWidget::showHelpForFileBrowser()
{
    if (HostOsInfo::isAnyUnixHost() && !HostOsInfo::isMacHost())
        showHelpDialog(tr("Variables"), UnixUtils::fileBrowserHelpText());
}

SystemSettings::SystemSettings()
{
    setId(Constants::SETTINGS_ID_SYSTEM);
    setDisplayName(SystemSettingsWidget::tr("System"));
    setCategory(Constants::SETTINGS_CATEGORY_CORE);
    setWidgetCreator([] { return new SystemSettingsWidget; });
}

} // namespace Internal
} // namespace Core
