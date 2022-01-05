#include "restartdialog.h"

#include <coreplugin/icore.h>

#include <QPushButton>

namespace Core {

RestartDialog::RestartDialog(QWidget *parent, const QString &text)
    : QMessageBox(parent)
{
    setWindowTitle(tr("Restart Required"));
    setText(text);
    setIcon(QMessageBox::Information);
    addButton(tr("Later"), QMessageBox::NoRole);
    addButton(tr("Restart Now"), QMessageBox::YesRole);

    connect(this, &QDialog::accepted, ICore::instance(), &ICore::restart, Qt::QueuedConnection);
}

} // namespace Core
