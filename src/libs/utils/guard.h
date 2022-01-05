#pragma once

#include "utils_global.h"
#include <QtGlobal>

namespace Utils {

class QTCREATOR_UTILS_EXPORT Guard
{
    Q_DISABLE_COPY(Guard)
public:
    Guard();
    ~Guard();
    bool isLocked() const;
private:
    int m_lockCount = 0;
    friend class GuardLocker;
};

class QTCREATOR_UTILS_EXPORT GuardLocker
{
    Q_DISABLE_COPY(GuardLocker)
public:
    GuardLocker(Guard &guard);
    ~GuardLocker();

private:
    Guard &m_guard;
};

} // namespace Utils
