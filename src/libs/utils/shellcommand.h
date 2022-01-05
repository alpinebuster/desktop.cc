#pragma once

#include "utils_global.h"

#include "qtcprocess.h"

QT_BEGIN_NAMESPACE
class QMutex;
class QVariant;
template <typename T>
class QFutureInterface;
template <typename T>
class QFuture;
QT_END_NAMESPACE

namespace Utils {

namespace Internal { class ShellCommandPrivate; }

class QTCREATOR_UTILS_EXPORT ProgressParser
{
public:
    ProgressParser();
    virtual ~ProgressParser();

protected:
    virtual void parseProgress(const QString &text) = 0;
    void setProgressAndMaximum(int value, int maximum);

private:
    void setFuture(QFutureInterface<void> *future);

    QFutureInterface<void> *m_future;
    QMutex *m_futureMutex = nullptr;
    friend class ShellCommand;
};

class QTCREATOR_UTILS_EXPORT ShellCommand : public QObject
{
    Q_OBJECT

public:
    // Convenience to synchronously run commands
    enum RunFlags {
        ShowStdOut = 0x1, // Show standard output.
        MergeOutputChannels = 0x2, // see QProcess: Merge stderr/stdout.
        SuppressStdErr = 0x4, // Suppress standard error output.
        SuppressFailMessage = 0x8, // No message about command failure.
        SuppressCommandLogging = 0x10, // No command log entry.
        ShowSuccessMessage = 0x20, // Show message about successful completion of command.
        ForceCLocale = 0x40, // Force C-locale for commands whose output is parsed.
        FullySynchronously = 0x80, // Suppress local event loop (in case UI actions are
                                   // triggered by file watchers).
        SilentOutput = 0x100, // Suppress user notifications about the output happening.
        NoFullySync = 0x200, // Avoid fully synchronous execution even in UI thread.
        NoOutput = SuppressStdErr | SuppressFailMessage | SuppressCommandLogging
    };


    ShellCommand(const QString &workingDirectory, const Environment &environment);
    ~ShellCommand() override;

    QString displayName() const;
    void setDisplayName(const QString &name);

    void addJob(const CommandLine &command,
                const QString &workingDirectory = QString(),
                const ExitCodeInterpreter &interpreter = {});
    void addJob(const CommandLine &command, int timeoutS,
                const QString &workingDirectory = QString(),
                const ExitCodeInterpreter &interpreter = {});
    void execute(); // Execute tasks asynchronously!
    void abort();
    bool lastExecutionSuccess() const;
    int lastExecutionExitCode() const;

    const QString &defaultWorkingDirectory() const;
    virtual const Environment processEnvironment() const;

    int defaultTimeoutS() const;
    void setDefaultTimeoutS(int timeout);

    unsigned flags() const;
    void addFlags(unsigned f);

    const QVariant &cookie() const;
    void setCookie(const QVariant &cookie);

    QTextCodec *codec() const;
    void setCodec(QTextCodec *codec);

    void setProgressParser(ProgressParser *parser);
    bool hasProgressParser() const;
    void setProgressiveOutput(bool progressive);

    void setDisableUnixTerminal();

    // This is called once per job in a thread.
    // When called from the UI thread it will execute fully synchronously, so no signals will
    // be triggered!
    virtual void runCommand(Utils::QtcProcess &process,
                            const CommandLine &command,
                            const QString &workingDirectory = QString());

    void cancel();

signals:
    void stdOutText(const QString &);
    void stdErrText(const QString &);
    void started();
    void finished(bool ok, int exitCode, const QVariant &cookie);
    void success(const QVariant &cookie);

    void terminate(); // Internal

    void append(const QString &text);
    void appendSilently(const QString &text);
    void appendError(const QString &text);
    void appendCommand(const QString &workingDirectory, const Utils::CommandLine &command);
    void appendMessage(const QString &text);

protected:
    virtual void addTask(QFuture<void> &future);
    int timeoutS() const;
    QString workDirectory(const QString &wd) const;

private:
    void run(QFutureInterface<void> &future);

    // Run without a event loop in fully blocking mode. No signals will be delivered.
    void runFullySynchronous(QtcProcess &proc,
                             const QString &workingDirectory);
    // Run with an event loop. Signals will be delivered.
    void runSynchronous(QtcProcess &proc,
                        const QString &workingDirectory);

    class Internal::ShellCommandPrivate *const d;
};

} // namespace Utils
