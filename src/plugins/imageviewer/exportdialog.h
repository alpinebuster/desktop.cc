#pragma once

#include <QDialog>

QT_FORWARD_DECLARE_CLASS(QSpinBox)

namespace Utils { class PathChooser; }

namespace ImageViewer {
namespace Internal {

struct ExportData;

class ExportDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ExportDialog(QWidget *parent = nullptr);

    QSize exportSize() const;
    void setExportSize(const QSize &);

    QString exportFileName() const;
    void setExportFileName(const QString &);

    ExportData exportData() const;

    void accept() override;

    static QString imageNameFilterString();

private:
    void resetExportSize();
    void exportWidthChanged(int width);
    void exportHeightChanged(int height);

    void setExportWidthBlocked(int width);
    void setExportHeightBlocked(int height);

    Utils::PathChooser *m_pathChooser;
    QSpinBox *m_widthSpinBox;
    QSpinBox *m_heightSpinBox;
    QSize m_defaultSize;
    qreal m_aspectRatio;
};

} // namespace Internal
} // namespace ImageViewer
