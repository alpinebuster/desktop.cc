#pragma once

#include <coreplugin/core_global.h>

#include <utils/id.h>
#include <utils/mimetypes/mimetype.h>

#include <QObject>

namespace Utils { class FilePath; }

namespace Core {

class IExternalHome;

using ExternalEditorList = QList<IExternalHome *>;

class CORE_EXPORT IExternalHome : public QObject
{
    Q_OBJECT

public:
    explicit IExternalHome(QObject *parent = nullptr);
    ~IExternalHome() override;

    static const ExternalEditorList allExternalEditors();
    static const ExternalEditorList externalEditors(const Utils::MimeType &mimeType);

    virtual QStringList mimeTypes() const = 0;
    virtual Utils::Id id() const = 0;
    virtual QString displayName() const = 0;
    virtual bool startEditor(const Utils::FilePath &filePath, QString *errorMessage) = 0;
};

} // namespace Core
