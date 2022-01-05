#pragma once

#include <QList>

namespace Core {
namespace Internal {

class CorePlugin;
class ProcessReaper;

class ProcessReapers final
{
private:
    ~ProcessReapers();
    ProcessReapers();

    QList<ProcessReaper *> m_reapers;

    friend class CorePlugin;
    friend class ProcessReaper;
};

} // namespace Internal
} // namespace Core
