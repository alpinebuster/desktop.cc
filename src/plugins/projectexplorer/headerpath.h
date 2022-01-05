#pragma once

#include <QString>
#include <QVector>

namespace ProjectExplorer {

enum class HeaderPathType {
    User,
    BuiltIn,
    System,
    Framework,
};

class HeaderPath
{
public:
    HeaderPath() = default;
    HeaderPath(const QString &path, HeaderPathType type)
        : path(path), type(type)
    { }

    bool operator==(const HeaderPath &other) const
    {
        return type == other.type && path == other.path;
    }

    bool operator!=(const HeaderPath &other) const
    {
        return !(*this == other);
    }

    QString path;
    HeaderPathType type = HeaderPathType::User;
};

inline auto qHash(const HeaderPath &key, uint seed = 0)
{
    return ((qHash(key.path) << 2) | uint(key.type)) ^ seed;
}

using HeaderPaths = QVector<HeaderPath>;
} // namespace ProjectExplorer
