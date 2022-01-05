#include "temporarydirectory.h"

#include "fileutils.h"

#include <QtCore/QCoreApplication>

#include "qtcassert.h"

namespace Utils {

static QTemporaryDir* m_masterTemporaryDir = nullptr;

static void cleanupMasterTemporaryDir()
{
    delete m_masterTemporaryDir;
    m_masterTemporaryDir = nullptr;
}

TemporaryDirectory::TemporaryDirectory(const QString &pattern) :
    QTemporaryDir(m_masterTemporaryDir->path() + '/' + pattern)
{
    QTC_CHECK(!QFileInfo(pattern).isAbsolute());
}

QTemporaryDir *TemporaryDirectory::masterTemporaryDirectory()
{
    return m_masterTemporaryDir;
}

void TemporaryDirectory::setMasterTemporaryDirectory(const QString &pattern)
{
    if (m_masterTemporaryDir)
        cleanupMasterTemporaryDir();
    else
        qAddPostRoutine(cleanupMasterTemporaryDir);
    m_masterTemporaryDir = new QTemporaryDir(pattern);
}

QString TemporaryDirectory::masterDirectoryPath()
{
    return m_masterTemporaryDir->path();
}

FilePath TemporaryDirectory::path() const
{
    return FilePath::fromString(QTemporaryDir::path());
}

FilePath TemporaryDirectory::filePath(const QString &fileName) const
{
    return FilePath::fromString(QTemporaryDir::filePath(fileName));
}

} // namespace Utils
