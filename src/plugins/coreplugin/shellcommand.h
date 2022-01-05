#pragma once

#include "core_global.h"

#include "progressmanager/futureprogress.h"

#include <utils/shellcommand.h>

#include <QPointer>

namespace Core {

class CORE_EXPORT ShellCommand : public Utils::ShellCommand
{
    Q_OBJECT

public:
    ShellCommand(const QString &workingDirectory, const Utils::Environment &environment);

    FutureProgress *futureProgress() const;

protected:
    void addTask(QFuture<void> &future) override;

    virtual void coreAboutToClose();

private:
    QPointer<FutureProgress> m_progress;
};

} // namespace Core
