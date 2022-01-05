#pragma once

#include "fileutils.h"
#include "hostosinfo.h"
#include "namevaluedictionary.h"
#include "namevalueitem.h"
#include "optional.h"

#include <QStringList>

#include <functional>

QT_FORWARD_DECLARE_CLASS(QProcessEnvironment)

namespace Utils {

class QTCREATOR_UTILS_EXPORT Environment final : public NameValueDictionary
{
public:
    using NameValueDictionary::NameValueDictionary;

    static Environment systemEnvironment();

    QProcessEnvironment toProcessEnvironment() const;

    void appendOrSet(const QString &key, const QString &value, const QString &sep = QString());
    void prependOrSet(const QString &key, const QString &value, const QString &sep = QString());

    void appendOrSetPath(const QString &value);
    void prependOrSetPath(const QString &value);

    void prependOrSetLibrarySearchPath(const QString &value);
    void prependOrSetLibrarySearchPaths(const QStringList &values);

    void setupEnglishOutput();

    using PathFilter = std::function<bool(const FilePath &)>;
    FilePath searchInPath(const QString &executable,
                          const FilePaths &additionalDirs = FilePaths(),
                          const PathFilter &func = PathFilter()) const;
    FilePaths findAllInPath(const QString &executable,
                               const FilePaths &additionalDirs = FilePaths(),
                               const PathFilter &func = PathFilter()) const;

    FilePaths path() const;
    FilePaths pathListValue(const QString &varName) const;
    QStringList appendExeExtensions(const QString &executable) const;

    bool isSameExecutable(const QString &exe1, const QString &exe2) const;

    QString expandedValueForKey(const QString &key) const;
    QString expandVariables(const QString &input) const;
    FilePath expandVariables(const FilePath &input) const;
    QStringList expandVariables(const QStringList &input) const;

    static void modifySystemEnvironment(const EnvironmentItems &list); // use with care!!!
    static void setSystemEnvironment(const Environment &environment);  // don't use at all!!!

private:
    static FilePath searchInDirectory(const QStringList &execs, const FilePath &directory,
                                      QSet<FilePath> &alreadyChecked);
};

class QTCREATOR_UTILS_EXPORT EnvironmentChange final
{
public:
    using Item = std::function<void(Environment &)>;

    EnvironmentChange() = default;

    void applyToEnvironment(Environment &) const;

    void addSetValue(const QString &key, const QString &value);
    void addUnsetValue(const QString &key);
    void addPrependToPath(const QString &value);
    void addAppendToPath(const QString &value);
    void addModify(const NameValueItems &items);
    void addChange(const Item &item) { m_changeItems.append(item); }

private:
    QList<Item> m_changeItems;
};

class QTCREATOR_UTILS_EXPORT EnvironmentProvider
{
public:
    QByteArray id;
    QString displayName;
    std::function<Environment()> environment;

    static void addProvider(EnvironmentProvider &&provider);
    static const QVector<EnvironmentProvider> providers();
    static optional<EnvironmentProvider> provider(const QByteArray &id);
};

} // namespace Utils
