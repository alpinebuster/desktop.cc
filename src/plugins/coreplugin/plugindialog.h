#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
QT_END_NAMESPACE

namespace ExtensionSystem {
class PluginSpec;
class PluginView;
}

namespace Core {
namespace Internal {

class PluginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginDialog(QWidget *parent);

private:
    void updateRestartRequired();
    void updateButtons();
    void openDetails(ExtensionSystem::PluginSpec *spec);
    void openErrorDetails();
    void closeDialog();
    void showInstallWizard();

    ExtensionSystem::PluginView *m_view;

    QPushButton *m_detailsButton;
    QPushButton *m_errorDetailsButton;
    QPushButton *m_installButton;
    QPushButton *m_closeButton;
    QLabel *m_restartRequired;
};

} // namespace Internal
} // namespace Core
