#include "launchersocket.h"
#include "launcherinterface.h"

#include "qtcassert.h"

#include <QtCore/qcoreapplication.h>
#include <QtNetwork/qlocalsocket.h>

namespace Utils {
namespace Internal {

class CallerHandle : public QObject
{
    Q_OBJECT
public:
    CallerHandle() : QObject() {}

    enum class SignalType {
        Started,
        ReadyRead,
        Finished
    };

    // always called in caller's thread
    void flush()
    {
        QList<SignalType> oldSignals;
        {
            QMutexLocker locker(&m_mutex);
            oldSignals = m_signals;
            m_signals = {};
        }
        for (SignalType signalType : qAsConst(oldSignals)) {
            switch (signalType) {
            case SignalType::Started:
                emit started();
                break;
            case SignalType::ReadyRead:
                emit readyRead();
                break;
            case SignalType::Finished:
                emit finished();
                break;
            }
        }
    }
    void appendSignal(SignalType signalType) { QMutexLocker locker(&m_mutex); m_signals.append(signalType); }
signals:
    void started();
    void readyRead();
    void finished();
private:
    QMutex m_mutex;
    QList<SignalType> m_signals;
};

void LauncherHandle::handlePacket(LauncherPacketType type, const QByteArray &payload)
{
    switch (type) {
    case LauncherPacketType::ProcessError:
        handleErrorPacket(payload);
        break;
    case LauncherPacketType::ProcessStarted:
        handleStartedPacket(payload);
        break;
    case LauncherPacketType::ProcessFinished:
        handleFinishedPacket(payload);
        break;
    default:
        QTC_ASSERT(false, break);
    }
}

void LauncherHandle::handleErrorPacket(const QByteArray &packetData)
{
    QMutexLocker locker(&m_mutex);
    if (!m_canceled)
        m_processState = QProcess::NotRunning;
    if (m_waitingFor != WaitingForState::Idle) {
        m_waitCondition.wakeOne();
        m_waitingFor = WaitingForState::Idle;
    }
    m_failed = true;

    const auto packet = LauncherPacket::extractPacket<ProcessErrorPacket>(m_token, packetData);
    m_error = packet.error;
    m_errorString = packet.errorString;
    // TODO: pipe it through the callers handle?
    emit errorOccurred(m_error);
}

// call me with mutex locked
void LauncherHandle::stateReached(WaitingForState wakeUpState, QProcess::ProcessState newState)
{
    if (!m_canceled)
        m_processState = newState;
    const bool shouldWake = m_waitingFor == wakeUpState;
    if (shouldWake) {
        m_waitCondition.wakeOne();
        m_waitingFor = WaitingForState::Idle;
    }
}

void LauncherHandle::sendPacket(const Internal::LauncherPacket &packet)
{
    LauncherInterface::socket()->sendData(packet.serialize());
}

// call me with mutex locked
void LauncherHandle::flushCaller()
{
    if (!m_callerHandle)
        return;

    // call in callers thread
    QMetaObject::invokeMethod(m_callerHandle, &CallerHandle::flush);
}

void LauncherHandle::handleStartedPacket(const QByteArray &packetData)
{
    QMutexLocker locker(&m_mutex);
    stateReached(WaitingForState::Started, QProcess::Running);
    if (m_canceled)
        return;
    const auto packet = LauncherPacket::extractPacket<ProcessStartedPacket>(m_token, packetData);
    m_processId = packet.processId;
    if (m_callerHandle) {
        m_callerHandle->appendSignal(CallerHandle::SignalType::Started);
        flushCaller();
    }
}

void LauncherHandle::handleFinishedPacket(const QByteArray &packetData)
{
    QMutexLocker locker(&m_mutex);
    stateReached(WaitingForState::Finished, QProcess::NotRunning);
    if (m_canceled)
        return;
    m_finished = true;
    const auto packet = LauncherPacket::extractPacket<ProcessFinishedPacket>(m_token, packetData);
    m_exitCode = packet.exitCode;
    m_stdout = packet.stdOut;
    m_stderr = packet.stdErr;
    m_errorString = packet.errorString;
    m_exitStatus = packet.exitStatus;
    if (m_callerHandle) {
        if (!m_stdout.isEmpty() || !m_stderr.isEmpty())
            m_callerHandle->appendSignal(CallerHandle::SignalType::ReadyRead);
        m_callerHandle->appendSignal(CallerHandle::SignalType::Finished);
        flushCaller();
    }
}

void LauncherHandle::handleSocketReady()
{
    QMutexLocker locker(&m_mutex);
    m_socketError = false;
    if (m_processState == QProcess::Starting)
        doStart();
}

void LauncherHandle::handleSocketError(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_socketError = true;
    m_errorString = QCoreApplication::translate("Utils::QtcProcess",
                                                "Internal socket error: %1").arg(message);
    if (m_processState != QProcess::NotRunning) {
        m_error = QProcess::FailedToStart;
        emit errorOccurred(m_error);
    }
}

bool LauncherHandle::waitForState(int msecs, WaitingForState newState, QProcess::ProcessState targetState)
{
    bool ok = false;
    {
        QMutexLocker locker(&m_mutex);
        // TODO: ASSERT if we are in Idle state
        if (m_canceled) // we don't want to wait if we have canceled it before (ASSERT it?)
            return false;

        if (m_processState == targetState) {
            qDebug() << "THE TARGET STATE IS ALREADY REACHED";
            ok = true;
        } else if (m_finished) { // it may happen, than after calling start() and before calling waitForStarted() we might have finished already
            qDebug() << "THE PROCESS HAS ALREADY FINISHED";
            ok = true;
        }
        if (!ok) {
            m_waitingFor = newState;
            ok = m_waitCondition.wait(&m_mutex, msecs) && !m_failed;
        }
    }
    if (ok) // since we are in caller's thread, m_callerHandle must be still valid
        m_callerHandle->flush();
    return ok;
}

void LauncherHandle::cancel()
{
    QMutexLocker locker(&m_mutex);

    switch (m_processState) {
    case QProcess::NotRunning:
        break;
    case QProcess::Starting:
        m_errorString = QCoreApplication::translate("Utils::LauncherHandle",
                                                    "Process canceled before it was started.");
        m_error = QProcess::FailedToStart;
        if (LauncherInterface::socket()->isReady())
            sendPacket(StopProcessPacket(m_token));
        else
            emit errorOccurred(m_error);
        break;
    case QProcess::Running:
        sendPacket(StopProcessPacket(m_token));
        break;
    }

    m_processState = QProcess::NotRunning;
    m_canceled = true;
}

void LauncherHandle::start(const QString &program, const QStringList &arguments, QIODevice::OpenMode mode)
{
    QMutexLocker locker(&m_mutex);

    if (m_socketError) {
        m_error = QProcess::FailedToStart;
        emit errorOccurred(m_error);
        return;
    }
    m_command = program;
    m_arguments = arguments;
    // TODO: check if state is not running
    // TODO: check if m_canceled is not true
    m_processState = QProcess::Starting;
    m_openMode = mode;
    if (LauncherInterface::socket()->isReady())
        doStart();
}

// Ensure it's called from caller's thread, after moving LauncherHandle into the launcher's thread
void LauncherHandle::createCallerHandle()
{
    QMutexLocker locker(&m_mutex); // may be not needed, as we call it just after Launcher's c'tor
    QTC_CHECK(m_callerHandle == nullptr);
    m_callerHandle = new CallerHandle();
    connect(m_callerHandle, &CallerHandle::started, this, &LauncherHandle::slotStarted, Qt::DirectConnection);
    connect(m_callerHandle, &CallerHandle::readyRead, this, &LauncherHandle::slotReadyRead, Qt::DirectConnection);
    connect(m_callerHandle, &CallerHandle::finished, this, &LauncherHandle::slotFinished, Qt::DirectConnection);
}

void LauncherHandle::destroyCallerHandle()
{
    QMutexLocker locker(&m_mutex);
    QTC_CHECK(m_callerHandle);
    m_callerHandle->deleteLater();
    m_callerHandle = nullptr;
}

void LauncherHandle::slotStarted()
{
    emit started();
}

void LauncherHandle::slotReadyRead()
{
    bool hasOutput = false;
    bool hasError = false;
    {
        QMutexLocker locker(&m_mutex);
        hasOutput = !m_stdout.isEmpty();
        hasError = !m_stderr.isEmpty();
    }
    if (hasOutput)
        emit readyReadStandardOutput();
    if (hasError)
        emit readyReadStandardError();
}

void LauncherHandle::slotFinished()
{
    int exitCode = 0;
    QProcess::ExitStatus exitStatus = QProcess::NormalExit;
    {
        QMutexLocker locker(&m_mutex);
        exitCode = m_exitCode;
        exitStatus = m_exitStatus;
    }
    emit finished(exitCode, exitStatus);
}

// call me with mutex locked
void LauncherHandle::doStart()
{
    StartProcessPacket p(m_token);
    p.command = m_command;
    p.arguments = m_arguments;
    p.env = m_environment.toStringList();
    p.workingDir = m_workingDirectory;
    p.openMode = m_openMode;
    p.channelMode = m_channelMode;
    sendPacket(p);
}

LauncherSocket::LauncherSocket(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<Utils::Internal::LauncherPacketType>();
    qRegisterMetaType<quintptr>("quintptr");
}

void LauncherSocket::sendData(const QByteArray &data)
{
    if (!isReady())
        return;
    QMutexLocker locker(&m_mutex);
    m_requests.push_back(data);
    if (m_requests.size() == 1)
        QMetaObject::invokeMethod(this, &LauncherSocket::handleRequests);
}

LauncherHandle *LauncherSocket::registerHandle(quintptr token)
{
    QMutexLocker locker(&m_mutex);
    if (m_handles.contains(token))
        return nullptr; // TODO: issue a warning

    LauncherHandle *handle = new LauncherHandle(token);
    handle->moveToThread(thread());
    // Call it after moving LauncherHandle to the launcher's thread.
    // Since this method is invoked from caller's thread, CallerHandle will live in caller's thread.
    handle->createCallerHandle();
    m_handles.insert(token, handle);
    connect(this, &LauncherSocket::ready,
            handle, &LauncherHandle::handleSocketReady);
    connect(this, &LauncherSocket::errorOccurred,
            handle, &LauncherHandle::handleSocketError);

    return handle;
}

void LauncherSocket::unregisterHandle(quintptr token)
{
    QMutexLocker locker(&m_mutex);
    auto it = m_handles.find(token);
    if (it == m_handles.end())
        return; // TODO: issue a warning

    LauncherHandle *handle = it.value();
    handle->destroyCallerHandle();
    handle->deleteLater();
    m_handles.erase(it);
}

LauncherHandle *LauncherSocket::handleForToken(quintptr token) const
{
    QMutexLocker locker(&m_mutex);
    return m_handles.value(token);
}

void LauncherSocket::shutdown()
{
    const auto socket = m_socket.exchange(nullptr);
    if (!socket)
        return;
    socket->disconnect();
    socket->write(ShutdownPacket().serialize());
    socket->waitForBytesWritten(1000);
    socket->deleteLater(); // or schedule a queued call to delete later?
}

void LauncherSocket::setSocket(QLocalSocket *socket)
{
    QTC_ASSERT(!m_socket, return);
    m_socket.store(socket);
    m_packetParser.setDevice(m_socket);
    connect(m_socket,
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
            static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error),
#else
            &QLocalSocket::errorOccurred,
#endif
            this, &LauncherSocket::handleSocketError);
    connect(m_socket, &QLocalSocket::readyRead,
            this, &LauncherSocket::handleSocketDataAvailable);
    connect(m_socket, &QLocalSocket::disconnected,
            this, &LauncherSocket::handleSocketDisconnected);
    emit ready();
}

void LauncherSocket::handleSocketError()
{
    auto socket = m_socket.load();
    if (socket->error() != QLocalSocket::PeerClosedError)
        handleError(QCoreApplication::translate("Utils::LauncherSocket",
                    "Socket error: %1").arg(socket->errorString()));
}

void LauncherSocket::handleSocketDataAvailable()
{
    try {
        if (!m_packetParser.parse())
            return;
    } catch (const PacketParser::InvalidPacketSizeException &e) {
        handleError(QCoreApplication::translate("Utils::LauncherSocket",
                    "Internal protocol error: invalid packet size %1.").arg(e.size));
        return;
    }
    LauncherHandle *handle = handleForToken(m_packetParser.token());
    if (handle) {
        switch (m_packetParser.type()) {
        case LauncherPacketType::ProcessError:
        case LauncherPacketType::ProcessStarted:
        case LauncherPacketType::ProcessFinished:
            handle->handlePacket(m_packetParser.type(), m_packetParser.packetData());
            break;
        default:
            handleError(QCoreApplication::translate("Utils::LauncherSocket",
                                                    "Internal protocol error: invalid packet type %1.")
                        .arg(static_cast<int>(m_packetParser.type())));
            return;
        }
    } else {
//        qDebug() << "No handler for token" << m_packetParser.token() << m_handles;
        // in this case the QtcProcess was canceled and deleted
    }
    handleSocketDataAvailable();
}

void LauncherSocket::handleSocketDisconnected()
{
    handleError(QCoreApplication::translate("Utils::LauncherSocket",
                "Launcher socket closed unexpectedly"));
}

void LauncherSocket::handleError(const QString &error)
{
    const auto socket = m_socket.exchange(nullptr);
    socket->disconnect();
    socket->deleteLater();
    emit errorOccurred(error);
}

void LauncherSocket::handleRequests()
{
    const auto socket = m_socket.load();
    QTC_ASSERT(socket, return);
    QMutexLocker locker(&m_mutex);
    for (const QByteArray &request : qAsConst(m_requests))
        socket->write(request);
    m_requests.clear();
}

} // namespace Internal
} // namespace Utils

#include "launchersocket.moc"
