#pragma once

//
//  W A R N I N G
//  -------------
//
// This file is mostly copied from Qt code and should not be touched
// unless really needed.
//


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qhash.h>
#include <QtCore/qmutex.h>
QT_BEGIN_NAMESPACE
class QFileInfo;
class QIODevice;
class QUrl;
QT_END_NAMESPACE

#include "mimetype.h"
#include "mimetype_p.h"
#include "mimeglobpattern_p.h"

namespace Utils {
namespace Internal {

class MimeProviderBase;

class MimeDatabasePrivate
{
public:
    Q_DISABLE_COPY(MimeDatabasePrivate)

    MimeDatabasePrivate();
    ~MimeDatabasePrivate();

    static MimeDatabasePrivate *instance();

    MimeProviderBase *provider();
    void setProvider(MimeProviderBase *theProvider);

    inline QString defaultMimeType() const { return m_defaultMimeType; }

    bool inherits(const QString &mime, const QString &parent);

    QList<MimeType> allMimeTypes();


    MimeType mimeTypeForName(const QString &nameOrAlias);
    MimeType mimeTypeForFileNameAndData(const QString &fileName, QIODevice *device, int *priorityPtr);
    MimeType findByData(const QByteArray &data, int *priorityPtr);
    QStringList mimeTypeForFileName(const QString &fileName, QString *foundSuffix = nullptr);

    mutable MimeProviderBase *m_provider;
    const QString m_defaultMimeType;
    QMutex mutex;

    int m_startupPhase = 0;
};

class MimeDatabase
{
    Q_DISABLE_COPY(MimeDatabase)

public:
    MimeDatabase();
    ~MimeDatabase();

    MimeType mimeTypeForName(const QString &nameOrAlias) const;

    enum MatchMode {
        MatchDefault = 0x0,
        MatchExtension = 0x1,
        MatchContent = 0x2
    };

    MimeType mimeTypeForFile(const QString &fileName, MatchMode mode = MatchDefault) const;
    MimeType mimeTypeForFile(const QFileInfo &fileInfo, MatchMode mode = MatchDefault) const;
    QList<MimeType> mimeTypesForFileName(const QString &fileName) const;

    MimeType mimeTypeForData(const QByteArray &data) const;
    MimeType mimeTypeForData(QIODevice *device) const;

    MimeType mimeTypeForUrl(const QUrl &url) const;
    MimeType mimeTypeForFileNameAndData(const QString &fileName, QIODevice *device) const;
    MimeType mimeTypeForFileNameAndData(const QString &fileName, const QByteArray &data) const;

    QString suffixForFileName(const QString &fileName) const;

    QList<MimeType> allMimeTypes() const;

    // For debugging purposes.
    enum StartupPhase {
        BeforeInitialize,
        PluginsLoading,
        PluginsInitializing, // Register up to here.
        PluginsDelayedInitializing, // Use from here on.
        UpAndRunning
    };
    static void setStartupPhase(StartupPhase);

private:
    Internal::MimeDatabasePrivate *d;
};

} // Internal
} // Utils
