#pragma once

#include "utils_global.h"

#include "hostosinfo.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QMetaType>
#include <QStringList>
#include <QUrl>
#include <QXmlStreamWriter> // Mac.

#include <functional>
#include <memory>

namespace Utils {
class Environment;
class FilePath;
} // Utils

QT_BEGIN_NAMESPACE
class QDataStream;
class QDateTime;
class QDir;
class QFile;
class QFileInfo;
class QTemporaryFile;
class QTextStream;
class QWidget;

QTCREATOR_UTILS_EXPORT QDebug operator<<(QDebug dbg, const Utils::FilePath &c);

// for withNtfsPermissions
#ifdef Q_OS_WIN
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

QT_END_NAMESPACE

// tst_fileutils becomes a friend of Utils::FilePath for testing private method
class tst_fileutils;

namespace Utils {

class DeviceFileHooks
{
public:
    std::function<bool(const FilePath &)> isExecutableFile;
    std::function<bool(const FilePath &)> isReadableFile;
    std::function<bool(const FilePath &)> isReadableDir;
    std::function<bool(const FilePath &)> isWritableDir;
    std::function<bool(const FilePath &)> isWritableFile;
    std::function<bool(const FilePath &)> isFile;
    std::function<bool(const FilePath &)> isDir;
    std::function<bool(const FilePath &)> ensureWritableDir;
    std::function<bool(const FilePath &)> ensureExistingFile;
    std::function<bool(const FilePath &)> createDir;
    std::function<bool(const FilePath &)> exists;
    std::function<bool(const FilePath &)> removeFile;
    std::function<bool(const FilePath &)> removeRecursively;
    std::function<bool(const FilePath &, const FilePath &)> copyFile;
    std::function<bool(const FilePath &, const FilePath &)> renameFile;
    std::function<FilePath(const FilePath &, const QList<FilePath> &)> searchInPath;
    std::function<FilePath(const FilePath &)> symLinkTarget;
    std::function<QList<FilePath>(const FilePath &, const QStringList &,
                                  QDir::Filters, QDir::SortFlags)> dirEntries;
    std::function<QByteArray(const FilePath &, qint64, qint64)> fileContents;
    std::function<bool(const FilePath &, const QByteArray &)> writeFileContents;
    std::function<QDateTime(const FilePath &)> lastModified;
    std::function<QFile::Permissions(const FilePath &)> permissions;
    std::function<OsType(const FilePath &)> osType;
    std::function<Environment(const FilePath &)> environment;
};

class QTCREATOR_UTILS_EXPORT FilePath
{
public:
    FilePath();

    static FilePath fromString(const QString &filepath);
    static FilePath fromFileInfo(const QFileInfo &info);
    static FilePath fromStringWithExtension(const QString &filepath, const QString &defaultExtension);
    static FilePath fromUserInput(const QString &filepath);
    static FilePath fromUtf8(const char *filepath, int filepathSize = -1);
    static FilePath fromVariant(const QVariant &variant);

    QString toString() const;
    FilePath onDevice(const FilePath &deviceTemplate) const;

    QFileInfo toFileInfo() const;
    QVariant toVariant() const;
    QDir toDir() const;

    QString toUserOutput() const;
    QString shortNativePath() const;

    QString fileName() const;
    QString fileNameWithPathComponents(int pathComponents) const;

    QString baseName() const;
    QString completeBaseName() const;
    QString suffix() const;
    QString completeSuffix() const;

    QString scheme() const { return m_scheme; }
    void setScheme(const QString &scheme);

    QString host() const { return m_host; }
    void setHost(const QString &host);

    QString path() const { return m_data; }
    void setPath(const QString &path) { m_data = path; }

    bool needsDevice() const;
    bool exists() const;

    bool isWritablePath() const { return isWritableDir(); } // Remove.
    bool isWritableDir() const;
    bool isWritableFile() const;
    bool ensureWritableDir() const;
    bool ensureExistingFile() const;
    bool isExecutableFile() const;
    bool isReadableFile() const;
    bool isReadableDir() const;
    bool isRelativePath() const;
    bool isAbsolutePath() const { return !isRelativePath(); }
    bool isFile() const;
    bool isDir() const;

    bool createDir() const;
    QList<FilePath> dirEntries(const QStringList &nameFilters,
                               QDir::Filters filters,
                               QDir::SortFlags sort = QDir::NoSort) const;
    QList<FilePath> dirEntries(QDir::Filters filters) const;
    QByteArray fileContents(qint64 maxSize = -1, qint64 offset = 0) const;
    bool writeFileContents(const QByteArray &data) const;

    FilePath parentDir() const;
    FilePath absolutePath() const;
    FilePath absoluteFilePath() const;
    FilePath absoluteFilePath(const FilePath &tail) const;
    FilePath absoluteFromRelativePath(const FilePath &anchor) const;

    bool operator==(const FilePath &other) const;
    bool operator!=(const FilePath &other) const;
    bool operator<(const FilePath &other) const;
    bool operator<=(const FilePath &other) const;
    bool operator>(const FilePath &other) const;
    bool operator>=(const FilePath &other) const;
    FilePath operator+(const QString &s) const;

    bool isChildOf(const FilePath &s) const;
    bool isChildOf(const QDir &dir) const;
    bool startsWith(const QString &s) const;
    bool endsWith(const QString &s) const;

    bool isNewerThan(const QDateTime &timeStamp) const;
    QDateTime lastModified() const;
    QFile::Permissions permissions() const;
    OsType osType() const;
    bool removeFile() const;
    bool removeRecursively(QString *error = nullptr) const;
    bool copyFile(const FilePath &target) const;
    bool renameFile(const FilePath &target) const;

    Qt::CaseSensitivity caseSensitivity() const;

    FilePath relativeChildPath(const FilePath &parent) const;
    FilePath relativePath(const FilePath &anchor) const;
    FilePath pathAppended(const QString &str) const;
    FilePath stringAppended(const QString &str) const;
    FilePath resolvePath(const QString &fileName) const;
    FilePath resolveSymlinkTarget() const;
    FilePath cleanPath() const;

    FilePath canonicalPath() const;
    FilePath symLinkTarget() const;
    FilePath resolveSymlinks() const;
    FilePath withExecutableSuffix() const;

    FilePath operator/(const QString &str) const;

    void clear();
    bool isEmpty() const;

    uint hash(uint seed) const;

    // NOTE: Most FilePath operations on FilePath created from URL currently
    // do not work. Among the working are .toVariant() and .toUrl().
    static FilePath fromUrl(const QUrl &url);
    QUrl toUrl() const;

    static void setDeviceFileHooks(const DeviceFileHooks &hooks);

    FilePath searchOnDevice(const QList<FilePath> &dirs) const;
    Environment deviceEnvironment() const;

private:
    friend class ::tst_fileutils;
    static QString calcRelativePath(const QString &absolutePath, const QString &absoluteAnchorPath);

    QString m_scheme;
    QString m_host;
    QString m_data;
};

QTCREATOR_UTILS_EXPORT QTextStream &operator<<(QTextStream &s, const FilePath &fn);

using FilePaths = QList<FilePath>;

class QTCREATOR_UTILS_EXPORT FileUtils {
public:
#ifdef QT_GUI_LIB
    class QTCREATOR_UTILS_EXPORT CopyAskingForOverwrite
    {
    public:
        CopyAskingForOverwrite(QWidget *dialogParent,
                               const std::function<void(QFileInfo)> &postOperation = {});
        bool operator()(const QFileInfo &src, const QFileInfo &dest, QString *error);
        QStringList files() const;

    private:
        QWidget *m_parent;
        QStringList m_files;
        std::function<void(QFileInfo)> m_postOperation;
        bool m_overwriteAll = false;
        bool m_skipAll = false;
    };
#endif // QT_GUI_LIB

    static bool copyRecursively(const FilePath &srcFilePath,
                                const FilePath &tgtFilePath,
                                QString *error = nullptr);
    template<typename T>
    static bool copyRecursively(const FilePath &srcFilePath,
                                const FilePath &tgtFilePath,
                                QString *error,
                                T &&copyHelper);
    static bool copyIfDifferent(const FilePath &srcFilePath,
                                const FilePath &tgtFilePath);
    static QString fileSystemFriendlyName(const QString &name);
    static int indexOfQmakeUnfriendly(const QString &name, int startpos = 0);
    static QString qmakeFriendlyName(const QString &name);
    static bool makeWritable(const FilePath &path);
    static QString normalizePathName(const QString &name);

    static bool isRelativePath(const QString &fileName);
    static bool isAbsolutePath(const QString &fileName) { return !isRelativePath(fileName); }
    static FilePath commonPath(const FilePath &oldCommonPath, const FilePath &fileName);
    static QByteArray fileId(const FilePath &fileName);
    static FilePath homePath();
    static bool renameFile(const FilePath &srcFilePath, const FilePath &tgtFilePath);
};

template<typename T>
bool FileUtils::copyRecursively(const FilePath &srcFilePath,
                                const FilePath &tgtFilePath,
                                QString *error,
                                T &&copyHelper)
{
    const QFileInfo srcFileInfo = srcFilePath.toFileInfo();
    if (srcFileInfo.isDir()) {
        if (!tgtFilePath.exists()) {
            const QDir targetDir(tgtFilePath.parentDir().toString());
            if (!targetDir.mkpath(tgtFilePath.fileName())) {
                if (error) {
                    *error = QCoreApplication::translate("Utils::FileUtils",
                                                         "Failed to create directory \"%1\".")
                                 .arg(tgtFilePath.toUserOutput());
                }
                return false;
            }
        }
        const QDir sourceDir(srcFilePath.toString());
        const QStringList fileNames = sourceDir.entryList(
            QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        for (const QString &fileName : fileNames) {
            const FilePath newSrcFilePath = srcFilePath / fileName;
            const FilePath newTgtFilePath = tgtFilePath / fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath, error, copyHelper))
                return false;
        }
    } else {
        if (!copyHelper(srcFileInfo, tgtFilePath.toFileInfo(), error))
            return false;
    }
    return true;
}

// for actually finding out if e.g. directories are writable on Windows
#ifdef Q_OS_WIN

template <typename T>
T withNtfsPermissions(const std::function<T()> &task)
{
    qt_ntfs_permission_lookup++;
    T result = task();
    qt_ntfs_permission_lookup--;
    return result;
}

template <>
QTCREATOR_UTILS_EXPORT void withNtfsPermissions(const std::function<void()> &task);

#else // Q_OS_WIN

template <typename T>
T withNtfsPermissions(const std::function<T()> &task)
{
    return task();
}

#endif // Q_OS_WIN

class QTCREATOR_UTILS_EXPORT FileReader
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils) // sic!
public:
    static QByteArray fetchQrc(const QString &fileName); // Only for internal resources
    bool fetch(const FilePath &filePath, QIODevice::OpenMode mode = QIODevice::NotOpen); // QIODevice::ReadOnly is implicit
    bool fetch(const FilePath &filePath, QIODevice::OpenMode mode, QString *errorString);
    bool fetch(const FilePath &filePath, QString *errorString)
        { return fetch(filePath, QIODevice::NotOpen, errorString); }
#ifdef QT_GUI_LIB
    bool fetch(const FilePath &filePath, QIODevice::OpenMode mode, QWidget *parent);
    bool fetch(const FilePath &filePath, QWidget *parent)
        { return fetch(filePath, QIODevice::NotOpen, parent); }
#endif // QT_GUI_LIB
    const QByteArray &data() const { return m_data; }
    const QString &errorString() const { return m_errorString; }
private:
    QByteArray m_data;
    QString m_errorString;
};

class QTCREATOR_UTILS_EXPORT FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils) // sic!
public:
    FileSaverBase();
    virtual ~FileSaverBase();

    FilePath filePath() const { return m_filePath; }
    bool hasError() const { return m_hasError; }
    QString errorString() const { return m_errorString; }
    virtual bool finalize();
    bool finalize(QString *errStr);
#ifdef QT_GUI_LIB
    bool finalize(QWidget *parent);
#endif

    bool write(const char *data, int len);
    bool write(const QByteArray &bytes);
    bool setResult(QTextStream *stream);
    bool setResult(QDataStream *stream);
    bool setResult(QXmlStreamWriter *stream);
    bool setResult(bool ok);

    QFile *file() { return m_file.get(); }

protected:
    std::unique_ptr<QFile> m_file;
    FilePath m_filePath;
    QString m_errorString;
    bool m_hasError = false;

private:
    Q_DISABLE_COPY(FileSaverBase)
};

class QTCREATOR_UTILS_EXPORT FileSaver : public FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils) // sic!
public:
    // QIODevice::WriteOnly is implicit
    explicit FileSaver(const FilePath &filePath, QIODevice::OpenMode mode = QIODevice::NotOpen);

    bool finalize() override;
    using FileSaverBase::finalize;

private:
    bool m_isSafe;
};

class QTCREATOR_UTILS_EXPORT TempFileSaver : public FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils) // sic!
public:
    explicit TempFileSaver(const QString &templ = QString());
    ~TempFileSaver() override;

    void setAutoRemove(bool on) { m_autoRemove = on; }

private:
    bool m_autoRemove = true;
};

inline uint qHash(const Utils::FilePath &a, uint seed = 0) { return a.hash(seed); }

} // namespace Utils

namespace std {
template<> struct QTCREATOR_UTILS_EXPORT hash<Utils::FilePath>
{
    using argument_type = Utils::FilePath;
    using result_type = size_t;
    result_type operator()(const argument_type &fn) const;
};
} // namespace std

Q_DECLARE_METATYPE(Utils::FilePath)
