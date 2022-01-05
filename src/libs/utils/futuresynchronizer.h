#pragma once

#include "utils_global.h"
#include <QtGlobal>

#include <QFuture>
#include <QList>

namespace Utils {

class QTCREATOR_UTILS_EXPORT FutureSynchronizer final
{
public:
    FutureSynchronizer() = default;
    ~FutureSynchronizer();

    template <typename T>
    void addFuture(const QFuture<T> &future)
    {
        m_futures.append(QFuture<void>(future));
        flushFinishedFutures();
    }

    bool isEmpty() const;

    void waitForFinished();
    void cancelAllFutures();
    void clearFutures();

    void setCancelOnWait(bool enabled);
    bool isCancelOnWait() const; // TODO: The original contained cancelOnWait, what suggests action, not a getter

    void flushFinishedFutures();

private:

    QList<QFuture<void>> m_futures;
    bool m_cancelOnWait = false; // TODO: True default makes more sense...
};

} // namespace Utils
