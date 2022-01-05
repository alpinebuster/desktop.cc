#pragma once

#include "utils_global.h"

#include <QObject>
#include <QThread>

QT_BEGIN_NAMESPACE
class QLocalServer;
QT_END_NAMESPACE

namespace Utils {
namespace Internal {
class LauncherProcess;
class LauncherSocket;
class LauncherInterfacePrivate;
}

class QTCREATOR_UTILS_EXPORT LauncherInterface : public QObject
{
    Q_OBJECT
public:
    static LauncherInterface &instance();
    ~LauncherInterface() override;

    static void startLauncher();
    static void stopLauncher();
    static Internal::LauncherSocket *socket();

signals:
    void errorOccurred(const QString &error);

private:
    LauncherInterface();

    QThread m_thread;
    Internal::LauncherInterfacePrivate *m_private;
};

} // namespace Utils
