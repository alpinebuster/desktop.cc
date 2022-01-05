#pragma once

#include <coreplugin/core_global.h>

#include <utils/id.h>
#include <utils/mimetypes/mimetype.h>

#include <QObject>
#include <QStringList>

#include <functional>

namespace Core {

class IEditor;
class IEditorFactory;

using HomeFactoryList = QList<IEditorFactory *>;

class CORE_EXPORT IEditorFactory : public QObject
{
    Q_OBJECT

public:
    IEditorFactory();
    ~IEditorFactory() override;

    static const HomeFactoryList allEditorFactories();
    static const HomeFactoryList defaultEditorFactories(const Utils::MimeType &mimeType);
    static const HomeFactoryList preferredEditorFactories(const QString &fileName);

    Utils::Id id() const { return m_id; }
    QString displayName() const { return m_displayName; }
    QStringList mimeTypes() const { return m_mimeTypes; }

    IEditor *createEditor() const;

protected:
    void setId(Utils::Id id) { m_id = id; }
    void setDisplayName(const QString &displayName) { m_displayName = displayName; }
    void setMimeTypes(const QStringList &mimeTypes) { m_mimeTypes = mimeTypes; }
    void addMimeType(const QString &mimeType) { m_mimeTypes.append(mimeType); }
    void setEditorCreator(const std::function<IEditor *()> &creator);

private:
    Utils::Id m_id;
    QString m_displayName;
    QStringList m_mimeTypes;
    std::function<IEditor *()> m_creator;
};

} // namespace Core
