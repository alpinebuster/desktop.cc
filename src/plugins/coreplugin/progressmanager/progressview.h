#pragma once

#include "progressmanager.h"

#include <QWidget>


QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace Core {

namespace Internal {

class ProgressView : public QWidget
{
    Q_OBJECT

public:
    ProgressView(QWidget *parent = nullptr);
    ~ProgressView() override;

    void addProgressWidget(QWidget *widget);
    void removeProgressWidget(QWidget *widget);

    bool isHovered() const;

    void setReferenceWidget(QWidget *widget);

protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void hoveredChanged(bool hovered);

private:
    void reposition();

    QVBoxLayout *m_layout;
    QWidget *m_referenceWidget = nullptr;
    bool m_hovered = false;
};

} // namespace Internal
} // namespace Core
