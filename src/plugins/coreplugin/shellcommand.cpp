#include "shellcommand.h"

#include "icore.h"
#include "progressmanager/progressmanager.h"

#include <QFutureInterface>
#include <QFutureWatcher>

namespace Core {

ShellCommand::ShellCommand(const QString &workingDirectory, const Utils::Environment &environment) :
    Utils::ShellCommand(workingDirectory, environment)
{
    connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose,
            this, &ShellCommand::coreAboutToClose);
}

FutureProgress *ShellCommand::futureProgress() const
{
    return m_progress.data();
}

void ShellCommand::addTask(QFuture<void> &future)
{
    const QString name = displayName();
    const auto id = Utils::Id::fromString(name + QLatin1String(".action"));
    if (hasProgressParser()) {
        m_progress = ProgressManager::addTask(future, name, id);
    } else {
        // add a timed tasked based on timeout
        // we cannot access the future interface directly, so we need to create a new one
        // with the same lifetime
        auto fi = new QFutureInterface<void>();
        auto watcher = new QFutureWatcher<void>();
        connect(watcher, &QFutureWatcherBase::finished, [fi, watcher] {
            fi->reportFinished();
            delete fi;
            watcher->deleteLater();
        });
        watcher->setFuture(future);
        m_progress = ProgressManager::addTimedTask(*fi, name, id, qMax(2, timeoutS() / 5)/*itsmagic*/);
    }
}

void ShellCommand::coreAboutToClose()
{
    abort();
}

} // namespace Core
