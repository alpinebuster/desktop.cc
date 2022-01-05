#pragma once

#include "core_global.h"

#include <utils/id.h>

#include <QStringList>

#include <functional>

namespace Core {

class IDocument;

class CORE_EXPORT IDocumentFactory
{
public:
    IDocumentFactory();
    ~IDocumentFactory();

    static const QList<IDocumentFactory *> allDocumentFactories();

    using Opener = std::function<IDocument *(const QString &fileName)>;
    IDocument *open(const QString &filename);

    QStringList mimeTypes() const { return m_mimeTypes; }

    void setOpener(const Opener &opener) { m_opener = opener; }
    void setMimeTypes(const QStringList &mimeTypes) { m_mimeTypes = mimeTypes; }
    void addMimeType(const char *mimeType) { m_mimeTypes.append(QLatin1String(mimeType)); }
    void addMimeType(const QString &mimeType) { m_mimeTypes.append(mimeType); }

private:
    Opener m_opener;
    QStringList m_mimeTypes;
    QString m_displayName;
};

} // namespace Core
