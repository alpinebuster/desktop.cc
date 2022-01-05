#pragma once

#include "utils_global.h"

#include <QProcess>
#include <QVector>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace Utils {

class Environment;
class CommandLine;

class QTCREATOR_UTILS_EXPORT TerminalCommand
{
public:
    TerminalCommand() = default;
    TerminalCommand(const QString &command, const QString &openArgs, const QString &executeArgs, bool needsQuotes = false);

    bool operator==(const TerminalCommand &other) const;
    bool operator<(const TerminalCommand &other) const;

    QString command;
    QString openArgs;
    QString executeArgs;
    bool needsQuotes = false;
};

class QTCREATOR_UTILS_EXPORT ConsoleProcess : public QObject
{
    Q_OBJECT

public:
    enum Mode { Run, Debug, Suspend };

    explicit ConsoleProcess(QObject *parent = nullptr);
    ~ConsoleProcess() override;

    void setCommand(const Utils::CommandLine &command);
    Utils::CommandLine command() const;

    void setAbortOnMetaChars(bool abort);

    void setWorkingDirectory(const QString &dir);
    QString workingDirectory() const;

    void setEnvironment(const Environment &env);
    Environment environment() const;

    void setRunAsRoot(bool on);

    QProcess::ProcessError error() const;
    QString errorString() const;

    bool start();
    void stop();

public:
    void setMode(Mode m);
    Mode mode() const;

    bool isRunning() const; // This reflects the state of the console+stub
    qint64 applicationPID() const;

    void kickoffProcess();
    void interruptProcess();
    void killProcess();
    void killStub();

    qint64 applicationMainThreadID() const;
    void detachStub();

    int exitCode() const;
    QProcess::ExitStatus exitStatus() const;

    void setSettings(QSettings *);

    static TerminalCommand defaultTerminalEmulator();
    static QVector<TerminalCommand> availableTerminalEmulators();
    static TerminalCommand terminalEmulator(const QSettings *settings);
    static void setTerminalEmulator(QSettings *settings, const TerminalCommand &term);

    static bool startTerminalEmulator(QSettings *settings, const QString &workingDir,
                                      const Utils::Environment &env);

signals:
    void errorOccurred(QProcess::ProcessError error);
    void processError(const QString &errorString);

    // These reflect the state of the actual client process
    void processStarted();
    void processStopped(int, QProcess::ExitStatus);

    // These reflect the state of the console+stub
    void stubStarted();
    void stubStopped();

private:
    void stubConnectionAvailable();
    void readStubOutput();
    void stubExited();

    static QString modeOption(Mode m);
    static QString msgCommChannelFailed(const QString &error);
    static QString msgPromptToClose();
    static QString msgCannotCreateTempFile(const QString &why);
    static QString msgCannotWriteTempFile();
    static QString msgCannotCreateTempDir(const QString & dir, const QString &why);
    static QString msgUnexpectedOutput(const QByteArray &what);
    static QString msgCannotChangeToWorkDir(const QString & dir, const QString &why);
    static QString msgCannotExecute(const QString & p, const QString &why);

    void emitError(QProcess::ProcessError err, const QString &errorString);
    QString stubServerListen();
    void stubServerShutdown();
    void cleanupStub();
    void cleanupInferior();

    class ConsoleProcessPrivate *d;
};

} // Utils

Q_DECLARE_METATYPE(Utils::TerminalCommand)
