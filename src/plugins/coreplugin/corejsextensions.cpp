#include "corejsextensions.h"

#include <app/app_version.h>

#include <utils/fileutils.h>
#include <utils/mimetypes/mimedatabase.h>
#include <utils/qtcassert.h>

#include <QDir>
#include <QTemporaryFile>
#include <QVariant>

namespace Core {
namespace Internal {

QString UtilsJsExtension::qtVersion() const
{
    return QLatin1String(qVersion());
}

QString UtilsJsExtension::qtCreatorVersion() const
{
    return QLatin1String(Constants::IDE_VERSION_DISPLAY);
}

QString UtilsJsExtension::toNativeSeparators(const QString &in) const
{
    return QDir::toNativeSeparators(in);
}

QString UtilsJsExtension::fromNativeSeparators(const QString &in) const
{
    return QDir::fromNativeSeparators(in);
}

QString UtilsJsExtension::baseName(const QString &in) const
{
    QFileInfo fi(in);
    return fi.baseName();
}

QString UtilsJsExtension::fileName(const QString &in) const
{
    QFileInfo fi(in);
    return fi.fileName();
}

QString UtilsJsExtension::completeBaseName(const QString &in) const
{
    QFileInfo fi(in);
    return fi.completeBaseName();
}

QString UtilsJsExtension::suffix(const QString &in) const
{
    QFileInfo fi(in);
    return fi.suffix();
}

QString UtilsJsExtension::completeSuffix(const QString &in) const
{
    QFileInfo fi(in);
    return fi.completeSuffix();
}

QString UtilsJsExtension::path(const QString &in) const
{
    QFileInfo fi(in);
    return fi.path();
}

QString UtilsJsExtension::absoluteFilePath(const QString &in) const
{
    QFileInfo fi(in);
    return fi.absoluteFilePath();
}

QString UtilsJsExtension::relativeFilePath(const QString &path, const QString &base) const
{
    return QDir(base).relativeFilePath(path);
}

bool UtilsJsExtension::exists(const QString &in) const
{
    return QFileInfo::exists(in);
}

bool UtilsJsExtension::isDirectory(const QString &in) const
{
    return QFileInfo(in).isDir();
}

bool UtilsJsExtension::isFile(const QString &in) const
{
    return QFileInfo(in).isFile();
}

QString UtilsJsExtension::preferredSuffix(const QString &mimetype) const
{
    Utils::MimeType mt = Utils::mimeTypeForName(mimetype);
    if (mt.isValid())
        return mt.preferredSuffix();
    return QString();
}

QString UtilsJsExtension::fileName(const QString &path, const QString &extension) const
{
    return Utils::FilePath::fromStringWithExtension(path, extension).toString();
}

QString UtilsJsExtension::mktemp(const QString &pattern) const
{
    QString tmp = pattern;
    if (tmp.isEmpty())
        tmp = QStringLiteral("qt_temp.XXXXXX");
    QFileInfo fi(tmp);
    if (!fi.isAbsolute()) {
        QString tempPattern = QDir::tempPath();
        if (!tempPattern.endsWith(QLatin1Char('/')))
            tempPattern += QLatin1Char('/');
        tmp = tempPattern + tmp;
    }

    QTemporaryFile file(tmp);
    file.setAutoRemove(false);
    QTC_ASSERT(file.open(), return QString());
    file.close();
    return file.fileName();
}

QString UtilsJsExtension::asciify(const QString &input) const
{
    QString result;
    for (const QChar &c : input) {
        if (c.isPrint() && c.unicode() < 128)
            result.append(c);
        else
            result.append(QString::fromLatin1("u%1").arg(c.unicode(), 4, 16, QChar('0')));
    }
    return result;
}

} // namespace Internal
} // namespace Core
