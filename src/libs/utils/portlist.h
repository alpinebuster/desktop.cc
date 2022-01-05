#pragma once

#include "utils_global.h"
#include "port.h"

QT_FORWARD_DECLARE_CLASS(QString)

namespace Utils {
namespace Internal { class PortListPrivate; }

class QTCREATOR_UTILS_EXPORT PortList
{
public:
    PortList();
    PortList(const PortList &other);
    PortList &operator=(const PortList &other);
    ~PortList();

    void addPort(Port port);
    void addRange(Port startPort, Port endPort);
    bool hasMore() const;
    bool contains(Port port) const;
    int count() const;
    Port getNext();
    QString toString() const;

    static PortList fromString(const QString &portsSpec);
    static QString regularExpression();

private:
    Internal::PortListPrivate * const d;
};

} // namespace Utils
