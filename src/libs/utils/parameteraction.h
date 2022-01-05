#pragma once

#include "utils_global.h"

#include <QAction>

namespace Utils {

class QTCREATOR_UTILS_EXPORT ParameterAction : public QAction
{
    Q_PROPERTY(QString emptyText READ emptyText WRITE setEmptyText)
    Q_PROPERTY(QString parameterText READ parameterText WRITE setParameterText)
    Q_PROPERTY(EnablingMode enablingMode READ enablingMode WRITE setEnablingMode)
    Q_OBJECT
public:
    enum EnablingMode { AlwaysEnabled, EnabledWithParameter };
    Q_ENUM(EnablingMode)

    explicit ParameterAction(const QString &emptyText,
                             const QString &parameterText,
                             EnablingMode em = AlwaysEnabled,
                             QObject *parent = nullptr);

    QString emptyText() const;
    void setEmptyText(const QString &);

    QString parameterText() const;
    void setParameterText(const QString &);

    EnablingMode enablingMode() const;
    void setEnablingMode(EnablingMode m);

public slots:
    void setParameter(const QString &);

private:
    QString m_emptyText;
    QString m_parameterText;
    EnablingMode m_enablingMode;
};

} // namespace Utils
