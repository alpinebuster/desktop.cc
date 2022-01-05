#pragma once

#include "utils_global.h"
#include "qtcassert.h"

#include <QMetaType>
#include <QString>

#include <limits>

namespace Utils {

class QTCREATOR_UTILS_EXPORT Port
{
public:
    Port() = default;
    explicit Port(quint16 port) : m_port(port) {}
    explicit Port(int port) :
        m_port((port < 0 || port > std::numeric_limits<quint16>::max()) ? -1 : port)
    {
    }

    explicit Port(uint port) :
        m_port(port > std::numeric_limits<quint16>::max() ? -1 : port)
    {
    }

    quint16 number() const { QTC_ASSERT(isValid(), return -1); return quint16(m_port); }
    bool isValid() const { return m_port != -1; }

    QString toString() const { return QString::number(m_port); }

private:
    int m_port = -1;
};

inline bool operator<(const Port &p1, const Port &p2) { return p1.number() < p2.number(); }
inline bool operator<=(const Port &p1, const Port &p2) { return p1.number() <= p2.number(); }
inline bool operator>(const Port &p1, const Port &p2) { return p1.number() > p2.number(); }
inline bool operator>=(const Port &p1, const Port &p2) { return p1.number() >= p2.number(); }

inline bool operator==(const Port &p1, const Port &p2)
{
    return p1.isValid() ? (p2.isValid() && p1.number() == p2.number()) : !p2.isValid();
}

inline bool operator!=(const Port &p1, const Port &p2)
{
    return p1.isValid() ? (!p2.isValid() || p1.number() != p2.number()) : p2.isValid();
}

} // Utils

Q_DECLARE_METATYPE(Utils::Port)
