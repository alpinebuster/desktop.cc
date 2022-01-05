#pragma once

#include "launcherpackets.h"

#include <QtCore/qobject.h>

#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include <QProcess>
#include <QWaitCondition>

#include <vector>
#include <atomic>

QT_BEGIN_NAMESPACE
class QLocalSocket;
QT_END_NAMESPACE

namespace Utils {
class LauncherInterface;

namespace Internal {

class LauncherInterfacePrivate;
class CallerHandle;

// Moved to the launcher thread, returned to caller's thread.
// It's assumed that this object will be alive at least
// as long as the corresponding QtcProcess is alive.

// We should have LauncherSocket::registerHandle() and LauncherSocket::unregisterHandle()
// methods.

class LauncherHandle : public QObject
{
    Q_OBJECT
public:
    // called from main thread
    bool waitForStarted(int msecs) // TODO: we might already be in finished state when calling this method - fix it!
    { return waitForState(msecs, WaitingForState::Started, QProcess::Running); }
    bool waitForFinished(int msecs)
    { return waitForState(msecs, WaitingForState::Finished, QProcess::NotRunning); }

    QProcess::ProcessState state() const
    { QMutexLocker locker(&m_mutex); return m_processState; }
    void cancel();
    bool isCanceled() const { return m_canceled; }

    QByteArray readAllStandardOutput()
    { QMutexLocker locker(&m_mutex); return readAndClear(m_stdout); }
    QByteArray readAllStandardError()
    { QMutexLocker locker(&m_mutex); return readAndClear(m_stderr); }

    qint64 processId() const { QMutexLocker locker(&m_mutex); return m_processId; }
    QString errorString() const { QMutexLocker locker(&m_mutex); return m_errorString; }
    void setErrorString(const QString &str) { QMutexLocker locker(&m_mutex); m_errorString = str; }

    // Called from other thread. Create a temp object receiver which lives in caller's thread.
    // Add started and finished signals to it and post a flush to it.
    // When we are in waitForState() which is called from the same thread,
    // we may flush the signal queue and emit these signals immediately.
    // Who should remove this object? deleteLater()?
    void start(const QString &program, const QStringList &arguments, QIODevice::OpenMode mode);

    QProcess::ProcessError error() const { QMutexLocker locker(&m_mutex); return m_error; }
    QString program() const { QMutexLocker locker(&m_mutex); return m_command; }
    void setProcessChannelMode(QProcess::ProcessChannelMode mode) {
        QMutexLocker locker(&m_mutex);
        if (mode != QProcess::SeparateChannels && mode != QProcess::MergedChannels) {
            qWarning("setProcessChannelMode: The only supported modes are SeparateChannels and MergedChannels.");
            return;
        }
        m_channelMode = mode;
    }
    void setProcessEnvironment(const QProcessEnvironment &environment)
    { QMutexLocker locker(&m_mutex); m_environment = environment; }
    void setWorkingDirectory(const QString &dir) { QMutexLocker locker(&m_mutex); m_workingDirectory = dir; }
    QProcess::ExitStatus exitStatus() const { QMutexLocker locker(&m_mutex); return m_exitStatus; }
signals:
    void errorOccurred(QProcess::ProcessError error);
    void started();
    void finished(int exitCode, QProcess::ExitStatus status);
    void readyReadStandardOutput();
    void readyReadStandardError();
private:
    enum class WaitingForState {
        Idle,
        Started,
        ReadyRead,
        Finished
    };
    // called from other thread
    bool waitForState(int msecs, WaitingForState newState, QProcess::ProcessState targetState);

    void doStart();

    void slotStarted();
    void slotReadyRead();
    void slotFinished();

    // called from this thread
    LauncherHandle(quintptr token) : m_token(token) {}
    void createCallerHandle();
    void destroyCallerHandle();

    void flushCaller();

    void handlePacket(LauncherPacketType type, const QByteArray &payload);
    void handleErrorPacket(const QByteArray &packetData);
    void handleStartedPacket(const QByteArray &packetData);
    void handleFinishedPacket(const QByteArray &packetData);

    void handleSocketReady();
    void handleSocketError(const QString &message);

    void stateReached(WaitingForState wakeUpState, QProcess::ProcessState newState);

    QByteArray readAndClear(QByteArray &data)
    {
        const QByteArray tmp = data;
        data.clear();
        return tmp;
    }

    void sendPacket(const Internal::LauncherPacket &packet);

    mutable QMutex m_mutex;
    QWaitCondition m_waitCondition;
    const quintptr m_token;
    WaitingForState m_waitingFor = WaitingForState::Idle;

    QProcess::ProcessState m_processState = QProcess::NotRunning;
    std::atomic_bool m_canceled = false;
    std::atomic_bool m_failed = false;
    std::atomic_bool m_finished = false;
    int m_processId = 0;
    int m_exitCode = 0;
    QProcess::ExitStatus m_exitStatus = QProcess::ExitStatus::NormalExit;
    QByteArray m_stdout;
    QByteArray m_stderr;
    QString m_errorString;
    QProcess::ProcessError m_error = QProcess::UnknownError;
    bool m_socketError = false;

    QString m_command;
    QStringList m_arguments;
    QProcessEnvironment m_environment;
    QString m_workingDirectory;
    QIODevice::OpenMode m_openMode = QIODevice::ReadWrite;
    QProcess::ProcessChannelMode m_channelMode = QProcess::SeparateChannels;

    CallerHandle *m_callerHandle = nullptr;

    friend class LauncherSocket;
};

class LauncherSocket : public QObject
{
    Q_OBJECT
    friend class LauncherInterfacePrivate;
public:
    bool isReady() const { return m_socket.load(); }
    void sendData(const QByteArray &data);

    LauncherHandle *registerHandle(quintptr token);
    void unregisterHandle(quintptr token);

signals:
    void ready();
    void errorOccurred(const QString &error);

private:
    LauncherSocket(QObject *parent = nullptr);

    LauncherHandle *handleForToken(quintptr token) const;

    void setSocket(QLocalSocket *socket);
    void shutdown();

    void handleSocketError();
    void handleSocketDataAvailable();
    void handleSocketDisconnected();
    void handleError(const QString &error);
    void handleRequests();

    std::atomic<QLocalSocket *> m_socket{nullptr};
    PacketParser m_packetParser;
    std::vector<QByteArray> m_requests;
    mutable QMutex m_mutex;
    QHash<quintptr, LauncherHandle *> m_handles;
};

} // namespace Internal
} // namespace Utils
