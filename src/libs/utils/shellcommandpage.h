#pragma once

#include "utils_global.h"

#include "wizardpage.h"

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QLabel;
QT_END_NAMESPACE

namespace Utils {
class OutputFormatter;
class ShellCommand;

class QTCREATOR_UTILS_EXPORT ShellCommandPage : public WizardPage
{
    Q_OBJECT

public:
    enum State { Idle, Running, Failed, Succeeded };

    explicit ShellCommandPage(QWidget *parent = nullptr);
    ~ShellCommandPage() override;

    void setStartedStatus(const QString &startedStatus);
    void start(ShellCommand *command);

    bool isComplete() const override;
    bool isRunning() const{ return m_state == Running; }

    void terminate();

    bool handleReject() override;

signals:
    void finished(bool success);

private:
    void slotFinished(bool ok, int exitCode, const QVariant &cookie);

    QPlainTextEdit *m_logPlainTextEdit;
    OutputFormatter *m_formatter;
    QLabel *m_statusLabel;

    ShellCommand *m_command = nullptr;
    QString m_startedStatus;
    bool m_overwriteOutput = false;

    State m_state = Idle;
};

} // namespace Utils
