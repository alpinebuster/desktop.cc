#include "globalfilechangeblocker.h"
#include "qtcassert.h"

#include <QApplication>

namespace Utils {

GlobalFileChangeBlocker::GlobalFileChangeBlocker()
{
    m_blockedState = QApplication::applicationState() != Qt::ApplicationActive;
    qApp->installEventFilter(this);
}

GlobalFileChangeBlocker *GlobalFileChangeBlocker::instance()
{
    static GlobalFileChangeBlocker blocker;
    return &blocker;
}

void GlobalFileChangeBlocker::forceBlocked(bool blocked)
{
    if (blocked)
        ++m_forceBlocked;
    else if (QTC_GUARD(m_forceBlocked > 0))
        --m_forceBlocked;
    emitIfChanged();
}

bool GlobalFileChangeBlocker::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == qApp && e->type() == QEvent::ApplicationStateChange)
        emitIfChanged();
    return false;
}

void GlobalFileChangeBlocker::emitIfChanged()
{
    const bool blocked = m_forceBlocked || (QApplication::applicationState() != Qt::ApplicationActive);
    if (blocked != m_blockedState) {
        emit stateChanged(blocked);
        m_blockedState = blocked;
    }
}

} // namespace Utils
