#pragma once

#include "utils_global.h"

#include <QObject>

namespace Utils {

class QTCREATOR_UTILS_EXPORT GlobalFileChangeBlocker : public QObject
{
    Q_OBJECT

public:
    static GlobalFileChangeBlocker *instance();
    void forceBlocked(bool blocked);
    bool isBlocked() const { return m_blockedState; }

signals:
    void stateChanged(bool blocked);

private:
    GlobalFileChangeBlocker();
    bool eventFilter(QObject *obj, QEvent *e) override;
    void emitIfChanged();

    int m_forceBlocked = 0;
    bool m_blockedState = false;
};

} // namespace Utils
