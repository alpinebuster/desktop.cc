#pragma once

#include "utils_global.h"

#include "environment.h"
#include "commandline.h"

#include <QProcess>
#include <QTextCodec>

#include <functional>

QT_FORWARD_DECLARE_CLASS(QDebug)

class tst_QtcProcess;

namespace Utils {

class CommandLine;
class Environment;
class QtcProcess;

namespace Internal { class QtcProcessPrivate; }

class DeviceProcessHooks
{
public:
    std::function<void(QtcProcess &)> startProcessHook;
    std::function<Environment(const FilePath &)> systemEnvironmentForBinary;
};

class QTCREATOR_UTILS_EXPORT QtcProcess : public QObject
{
    Q_OBJECT

public:
    enum ProcessImpl {
        QProcessImpl,
        ProcessLauncherImpl
    };

    QtcProcess(ProcessImpl processImpl, QObject *parent = nullptr);
    QtcProcess(QObject *parent = nullptr);
    ~QtcProcess();

    enum Result {
        // Finished successfully. Unless an ExitCodeInterpreter is set
        // this corresponds to a return code 0.
        FinishedWithSuccess,
        Finished = FinishedWithSuccess, // FIXME: Kept to ease downstream transition
        // Finished unsuccessfully. Unless an ExitCodeInterpreter is set
        // this corresponds to a return code different from 0.
        FinishedWithError,
        FinishedError = FinishedWithError, // FIXME: Kept to ease downstream transition
        // Process terminated abnormally (kill)
        TerminatedAbnormally,
        // Executable could not be started
        StartFailed,
        // Hang, no output after time out
        Hang
    };

    void setEnvironment(const Environment &env);
    const Environment &environment() const;

    void setCommand(const CommandLine &cmdLine);
    const CommandLine &commandLine() const;

    FilePath workingDirectory() const;
    void setWorkingDirectory(const FilePath &dir);
    void setWorkingDirectory(const QString &dir); // FIXME: Kept to ease downstream transition

    void setUseCtrlCStub(bool enabled);
    void setLowPriority();
    void setDisableUnixTerminal();

    void start();
    void start(const QString &cmd, const QStringList &args = {});
    void terminate();
    void interrupt();

    // Starts the command and waits for finish. User input processing depends
    // on whether setProcessUserEventWhileRunning was called.
    void runBlocking();
    // This starts a nested event loop when running the command.
    void setProcessUserEventWhileRunning(); // Avoid.

    /* Timeout for hanging processes (triggers after no more output
     * occurs on stderr/stdout). */
    void setTimeoutS(int timeoutS);

    void setCodec(QTextCodec *c);
    void setTimeOutMessageBoxEnabled(bool);
    void setExitCodeInterpreter(const std::function<QtcProcess::Result(int)> &interpreter);

    // FIXME: This is currently only used in run(), not in start()
    void setWriteData(const QByteArray &writeData);

    void setStdOutCallback(const std::function<void(const QString &)> &callback);
    void setStdOutLineCallback(const std::function<void(const QString &)> &callback);
    void setStdErrCallback(const std::function<void(const QString &)> &callback);
    void setStdErrLineCallback(const std::function<void(const QString &)> &callback);

    static void setRemoteProcessHooks(const DeviceProcessHooks &hooks);

    void setOpenMode(QIODevice::OpenMode mode);

    bool stopProcess();
    bool readDataFromProcess(int timeoutS, QByteArray *stdOut, QByteArray *stdErr,
                             bool showTimeOutMessageBox);

    static QString normalizeNewlines(const QString &text);

    Result result() const;
    void setResult(Result result);

    QByteArray allRawOutput() const;
    QString allOutput() const;

    QString stdOut() const;
    QString stdErr() const;

    QByteArray rawStdOut() const;

    int exitCode() const;

    QString exitMessage();

    // Helpers to find binaries. Do not use it for other path variables
    // and file types.
    static QString locateBinary(const QString &binary);
    static QString locateBinary(const QString &path, const QString &binary);

    static Environment systemEnvironmentForBinary(const FilePath &filePath);

    // FIXME: Cut down the following bits inherited from QProcess and QIODevice.

    void setProcessChannelMode(QProcess::ProcessChannelMode mode);

    QProcess::ProcessError error() const;
    QProcess::ProcessState state() const;

    QString errorString() const;
    void setErrorString(const QString &str);

    qint64 processId() const;

    bool waitForStarted(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000);
    bool waitForFinished(int msecs = 30000);

    QByteArray readAllStandardOutput();
    QByteArray readAllStandardError();

    QProcess::ExitStatus exitStatus() const;

    void kill();

    qint64 write(const QByteArray &input);
    void closeWriteChannel();
    void close();

    void setKeepWriteChannelOpen();
    bool keepsWriteChannelOpen() const;

    void setStandardInputFile(const QString &inputFile);

signals:
    void started();
    void finished();
    void errorOccurred(QProcess::ProcessError error);
    void readyReadStandardOutput();
    void readyReadStandardError();

private:
    friend QTCREATOR_UTILS_EXPORT QDebug operator<<(QDebug str, const QtcProcess &r);

    Internal::QtcProcessPrivate *d = nullptr;

    friend tst_QtcProcess;
    void beginFeed();
    void feedStdOut(const QByteArray &data);
    void endFeed();

    void setProcessEnvironment(const QProcessEnvironment &environment) = delete;
    QProcessEnvironment processEnvironment() const = delete;
};

using ExitCodeInterpreter = std::function<QtcProcess::Result(int /*exitCode*/)>;

QTCREATOR_UTILS_EXPORT QDebug operator<<(QDebug str, const QtcProcess &);

using SynchronousProcess = QtcProcess; // FIXME: Remove.

} // namespace Utils
