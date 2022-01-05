#pragma once

#include "fileutils.h"
#include "utils_global.h"

#include <QVariant>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace Utils {

class QTCREATOR_UTILS_EXPORT PersistentSettingsReader
{
public:
    PersistentSettingsReader();
    QVariant restoreValue(const QString &variable, const QVariant &defaultValue = QVariant()) const;
    QVariantMap restoreValues() const;
    bool load(const FilePath &fileName);

private:
    QMap<QString, QVariant> m_valueMap;
};

class QTCREATOR_UTILS_EXPORT PersistentSettingsWriter
{
public:
    PersistentSettingsWriter(const FilePath &fileName, const QString &docType);

    bool save(const QVariantMap &data, QString *errorString) const;
#ifdef QT_GUI_LIB
    bool save(const QVariantMap &data, QWidget *parent) const;
#endif

    FilePath fileName() const;

    void setContents(const QVariantMap &data);

private:
    bool write(const QVariantMap &data, QString *errorString) const;

    const FilePath m_fileName;
    const QString m_docType;
    mutable QMap<QString, QVariant> m_savedData;
};

} // namespace Utils
