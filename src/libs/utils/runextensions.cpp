#include "runextensions.h"

namespace Utils {
namespace Internal {

RunnableThread::RunnableThread(QRunnable *runnable, QObject *parent)
    : QThread(parent),
      m_runnable(runnable)
{
}

void RunnableThread::run()
{
    m_runnable->run();
    if (m_runnable->autoDelete())
        delete m_runnable;
}

} // Internal
} // Utils
