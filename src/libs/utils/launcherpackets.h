#pragma once

#include <QtCore/qdatastream.h>
#include <QtCore/qprocess.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE
class QByteArray;
QT_END_NAMESPACE

namespace Utils {
namespace Internal {

enum class LauncherPacketType {
    Shutdown, StartProcess, ProcessStarted, StopProcess, ProcessError, ProcessFinished
};

class PacketParser
{
public:
    class InvalidPacketSizeException
    {
    public:
        InvalidPacketSizeException(int size) : size(size) { }
        const int size;
    };

    void setDevice(QIODevice *device);
    bool parse();
    LauncherPacketType type() const { return m_type; }
    quintptr token() const { return m_token; }
    const QByteArray &packetData() const { return m_packetData; }

private:
    QDataStream m_stream;
    LauncherPacketType m_type = LauncherPacketType::Shutdown;
    quintptr m_token = 0;
    QByteArray m_packetData;
    int m_sizeOfNextPacket = -1;
};

class LauncherPacket
{
public:
    virtual ~LauncherPacket();

    template<class Packet> static Packet extractPacket(quintptr token, const QByteArray &data)
    {
        Packet p(token);
        p.deserialize(data);
        return p;
    }

    QByteArray serialize() const;
    void deserialize(const QByteArray &data);

    const LauncherPacketType type;
    const quintptr token = 0;

protected:
    LauncherPacket(LauncherPacketType type, quintptr token) : type(type), token(token) { }

private:
    virtual void doSerialize(QDataStream &stream) const = 0;
    virtual void doDeserialize(QDataStream &stream) = 0;
};

class StartProcessPacket : public LauncherPacket
{
public:
    StartProcessPacket(quintptr token);

    QString command;
    QStringList arguments;
    QString workingDir;
    QStringList env;
    QIODevice::OpenMode openMode = QIODevice::ReadWrite;
    QProcess::ProcessChannelMode channelMode = QProcess::SeparateChannels;

private:
    void doSerialize(QDataStream &stream) const override;
    void doDeserialize(QDataStream &stream) override;
};

class ProcessStartedPacket : public LauncherPacket
{
public:
    ProcessStartedPacket(quintptr token);

    int processId = 0;

private:
    void doSerialize(QDataStream &stream) const override;
    void doDeserialize(QDataStream &stream) override;
};

class StopProcessPacket : public LauncherPacket
{
public:
    StopProcessPacket(quintptr token);

private:
    void doSerialize(QDataStream &stream) const override;
    void doDeserialize(QDataStream &stream) override;
};

class ShutdownPacket : public LauncherPacket
{
public:
    ShutdownPacket();

private:
    void doSerialize(QDataStream &stream) const override;
    void doDeserialize(QDataStream &stream) override;
};

class ProcessErrorPacket : public LauncherPacket
{
public:
    ProcessErrorPacket(quintptr token);

    QProcess::ProcessError error = QProcess::UnknownError;
    QString errorString;

private:
    void doSerialize(QDataStream &stream) const override;
    void doDeserialize(QDataStream &stream) override;
};

class ProcessFinishedPacket : public LauncherPacket
{
public:
    ProcessFinishedPacket(quintptr token);

    QString errorString;
    QByteArray stdOut;
    QByteArray stdErr;
    QProcess::ExitStatus exitStatus = QProcess::ExitStatus::NormalExit;
    QProcess::ProcessError error = QProcess::ProcessError::UnknownError;
    int exitCode = 0;

private:
    void doSerialize(QDataStream &stream) const override;
    void doDeserialize(QDataStream &stream) override;
};

} // namespace Internal
} // namespace Utils

Q_DECLARE_METATYPE(Utils::Internal::LauncherPacketType);
