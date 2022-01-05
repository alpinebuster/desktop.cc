#include "filecrumblabel.h"

#include <utils/hostosinfo.h>

#include <QDir>
#include <QUrl>

namespace Utils {

FileCrumbLabel::FileCrumbLabel(QWidget *parent)
    : QLabel(parent)
{
    setTextFormat(Qt::RichText);
    setWordWrap(true);
    connect(this, &QLabel::linkActivated, this, [this](const QString &url) {
        emit pathClicked(FilePath::fromString(QUrl(url).toLocalFile()));
    });
    setPath(FilePath());
}

static QString linkForPath(const FilePath &path, const QString &display)
{
    return "<a href=\""
            + QUrl::fromLocalFile(path.toString()).toString(QUrl::FullyEncoded) + "\">"
            + display + "</a>";
}

void FileCrumbLabel::setPath(const FilePath &path)
{
    QStringList links;
    FilePath current = path;
    while (!current.isEmpty()) {
        const QString fileName = current.fileName();
        if (!fileName.isEmpty()) {
            links.prepend(linkForPath(current, fileName));
        } else if (HostOsInfo::isWindowsHost() && QDir(current.toString()).isRoot()) {
            // Only on Windows add the drive letter, without the '/' at the end
            QString display = current.toString();
            if (display.endsWith('/'))
                display.chop(1);
            links.prepend(linkForPath(current, display));
        }
        current = current.parentDir();
    }
    const auto pathSeparator = HostOsInfo::isWindowsHost() ? QLatin1String("&nbsp;\\ ")
                                                           : QLatin1String("&nbsp;/ ");
    const QString prefix = HostOsInfo::isWindowsHost() ? QString("\\ ") : QString("/ ");
    setText(prefix + links.join(pathSeparator));
}

} // Utils
