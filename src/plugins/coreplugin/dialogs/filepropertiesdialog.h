#pragma once

#include <utils/fileutils.h>

#include <QDialog>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui {
class FilePropertiesDialog;
}
QT_END_NAMESPACE

class FilePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilePropertiesDialog(const Utils::FilePath &fileName, QWidget *parent = nullptr);
    ~FilePropertiesDialog() override;

private:
    void refresh();
    void setPermission(QFile::Permissions newPermissions, bool set);
    void detectTextFileSettings();

private:
    Ui::FilePropertiesDialog *m_ui = nullptr;
    const QString m_fileName;
};
