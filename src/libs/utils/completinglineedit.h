#pragma once

#include "utils_global.h"
#include <QLineEdit>

namespace Utils {

class QTCREATOR_UTILS_EXPORT CompletingLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit CompletingLineEdit(QWidget *parent = nullptr);

protected:
    bool event(QEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
};

} // namespace Utils
