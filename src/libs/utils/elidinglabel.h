#pragma once

#include "utils_global.h"
#include <QLabel>

namespace Utils {

class  QTCREATOR_UTILS_EXPORT ElidingLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(Qt::TextElideMode elideMode READ elideMode WRITE setElideMode DESIGNABLE true)

public:
    explicit ElidingLabel(QWidget *parent = nullptr);
    explicit ElidingLabel(const QString &text, QWidget *parent = nullptr);

    Qt::TextElideMode elideMode() const;
    void setElideMode(const Qt::TextElideMode &elideMode);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Qt::TextElideMode m_elideMode;
};

} // namespace Utils
