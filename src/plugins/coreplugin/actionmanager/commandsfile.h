#pragma once

#include <QObject>
#include <QString>
#include <QMap>

QT_FORWARD_DECLARE_CLASS(QKeySequence)

namespace Core {
namespace Internal {

struct ShortcutItem;

class CommandsFile : public QObject
{
    Q_OBJECT

public:
    CommandsFile(const QString &filename);

    QMap<QString, QList<QKeySequence> > importCommands() const;
    bool exportCommands(const QList<ShortcutItem *> &items);

private:
    QString m_filename;
};

} // namespace Internal
} // namespace Core
