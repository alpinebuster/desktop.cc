#include "reaper.h"
#include "reaper_p.h"

#include <utils/algorithm.h>
#include <utils/qtcassert.h>
#include <utils/qtcprocess.h>

#include <QProcess>
#include <QThread>
#include <QTimer>

using namespace Utils;

namespace Core {
namespace Internal {

static ProcessReapers *d = nullptr;

class ProcessReaper final : public QObject
{
public:
    ProcessReaper(QtcProcess *p, int timeoutMs);
    ~ProcessReaper() final;

    int timeoutMs() const;
    bool isFinished() const;
    void nextIteration();

private:
    mutable QTimer m_iterationTimer;
    QtcProcess *m_process;
    int m_emergencyCounter = 0;
    QProcess::ProcessState m_lastState = QProcess::NotRunning;
};

ProcessReaper::ProcessReaper(QtcProcess *p, int timeoutMs) : m_process(p)
{
    d->m_reapers.append(this);

    m_iterationTimer.setInterval(timeoutMs);
    m_iterationTimer.setSingleShot(true);
    connect(&m_iterationTimer, &QTimer::timeout, this, &ProcessReaper::nextIteration);

    QMetaObject::invokeMethod(this, &ProcessReaper::nextIteration, Qt::QueuedConnection);
}

ProcessReaper::~ProcessReaper()
{
    d->m_reapers.removeOne(this);
}

int ProcessReaper::timeoutMs() const
{
    const int remaining = m_iterationTimer.remainingTime();
    if (remaining < 0)
        return m_iterationTimer.interval();
    m_iterationTimer.stop();
    return remaining;
}

bool ProcessReaper::isFinished() const
{
    return !m_process;
}

void ProcessReaper::nextIteration()
{
    QProcess::ProcessState state = m_process ? m_process->state() : QProcess::NotRunning;
    if (state == QProcess::NotRunning || m_emergencyCounter > 5) {
        delete m_process;
        m_process = nullptr;
        return;
    }

    if (state == QProcess::Starting) {
        if (m_lastState == QProcess::Starting)
            m_process->kill();
    } else if (state == QProcess::Running) {
        if (m_lastState == QProcess::Running)
            m_process->kill();
        else
            m_process->terminate();
    }

    m_lastState = state;
    m_iterationTimer.start();

    ++m_emergencyCounter;
}

ProcessReapers::ProcessReapers()
{
    d = this;
}

ProcessReapers::~ProcessReapers()
{
    while (!m_reapers.isEmpty()) {
        int alreadyWaited = 0;
        QList<ProcessReaper *> toDelete;

        // push reapers along:
        foreach (ProcessReaper *pr, m_reapers) {
            const int timeoutMs = pr->timeoutMs();
            if (alreadyWaited < timeoutMs) {
                const unsigned long toSleep = static_cast<unsigned long>(timeoutMs - alreadyWaited);
                QThread::msleep(toSleep);
                alreadyWaited += toSleep;
            }

            pr->nextIteration();

            if (pr->isFinished())
                toDelete.append(pr);
        }

        // Clean out reapers that finished in the meantime
        qDeleteAll(toDelete);
        toDelete.clear();
    }

    d = nullptr;
}

} // namespace Internal

namespace Reaper {

void reap(QtcProcess *process, int timeoutMs)
{
    if (!process)
        return;

    QTC_ASSERT(Internal::d, return);

    new Internal::ProcessReaper(process, timeoutMs);
}

} // namespace Reaper
} // namespace Core

