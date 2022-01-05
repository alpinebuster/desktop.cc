#pragma once

#include "utils_global.h"

#include <QMetaType>

namespace Utils {

class QTCREATOR_UTILS_EXPORT ProcessHandle
{
public:
    ProcessHandle();
    explicit ProcessHandle(qint64 pid);

    bool isValid() const;
    void setPid(qint64 pid);
    qint64 pid() const;

    bool equals(const ProcessHandle &) const;

    bool activate();

private:
    qint64 m_pid;
};

inline bool operator==(const ProcessHandle &p1, const ProcessHandle &p2) { return p1.equals(p2); }
inline bool operator!=(const ProcessHandle &p1, const ProcessHandle &p2) { return !p1.equals(p2); }

} // Utils

Q_DECLARE_METATYPE(Utils::ProcessHandle)
