#include "removefiledialog.h"
#include "ui_removefiledialog.h"

#include <QDir>

namespace Utils {

RemoveFileDialog::RemoveFileDialog(const QString &filePath, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::RemoveFileDialog)
{
    m_ui->setupUi(this);
    m_ui->fileNameLabel->setText(QDir::toNativeSeparators(filePath));

    // TODO
    m_ui->removeVCCheckBox->setVisible(false);
}

RemoveFileDialog::~RemoveFileDialog()
{
    delete m_ui;
}

void RemoveFileDialog::setDeleteFileVisible(bool visible)
{
    m_ui->deleteFileCheckBox->setVisible(visible);
}

bool RemoveFileDialog::isDeleteFileChecked() const
{
    return m_ui->deleteFileCheckBox->isChecked();
}

} // Utils
