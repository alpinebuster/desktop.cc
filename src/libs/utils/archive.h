#pragma once

#include "utils_global.h"

#include "fileutils.h"

#include <QObject>

namespace Utils {

class QtcProcess;

class QTCREATOR_UTILS_EXPORT Archive : public QObject
{
    Q_OBJECT
public:
    static bool supportsFile(const FilePath &filePath, QString *reason = nullptr);
    static bool unarchive(const FilePath &src, const FilePath &dest, QWidget *parent);
    static Archive *unarchive(const FilePath &src, const FilePath &dest);

    void cancel();

signals:
    void outputReceived(const QString &output);
    void finished(bool success);

private:
    Archive() = default;

    QtcProcess *m_process = nullptr;
};

} // namespace Utils
