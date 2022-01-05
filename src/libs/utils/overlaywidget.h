#pragma once

#include "utils_global.h"

#include <QWidget>

#include <functional>

namespace Utils {

class QTCREATOR_UTILS_EXPORT OverlayWidget : public QWidget
{
    Q_OBJECT
public:
    using PaintFunction = std::function<void(QWidget *, QPainter &, QPaintEvent *)>;

    explicit OverlayWidget(QWidget *parent = nullptr);

    void attachToWidget(QWidget *parent);
    void setPaintFunction(const PaintFunction &paint);

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;
    void paintEvent(QPaintEvent *ev) override;

private:
    void resizeToParent();

    PaintFunction m_paint;
};

} // namespace Utils
