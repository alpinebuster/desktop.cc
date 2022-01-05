#pragma once

#include "utils_global.h"

#include <QTemporaryFile>

#include <memory>

namespace Utils {

class QTCREATOR_UTILS_EXPORT SaveFile : public QFile
{
    Q_OBJECT

public:
    explicit SaveFile(const QString &filename);
    ~SaveFile() override;

    bool open(OpenMode flags = QIODevice::WriteOnly) override;

    void rollback();
    bool commit();

    static void initializeUmask();

private:
    const QString m_finalFileName;
    std::unique_ptr<QTemporaryFile> m_tempFile;
    bool m_finalized = true;
};

} // namespace Utils
