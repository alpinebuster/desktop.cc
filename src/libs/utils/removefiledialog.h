#pragma once

#include "utils_global.h"

#include <QDialog>

namespace Utils {

namespace Ui { class RemoveFileDialog; }

class QTCREATOR_UTILS_EXPORT RemoveFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveFileDialog(const QString &filePath, QWidget *parent = nullptr);
    ~RemoveFileDialog() override;

    void setDeleteFileVisible(bool visible);
    bool isDeleteFileChecked() const;

private:
    Ui::RemoveFileDialog *m_ui;
};

} // namespace Utils
