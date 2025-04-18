// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "environmentmodel.h"

#include "algorithm.h"
#include "environment.h"
#include "hostosinfo.h"
#include "namevaluedictionary.h"
#include "namevalueitem.h"
#include "qtcassert.h"
#include "utilstr.h"

#include <QFileInfo>
#include <QFont>
#include <QGuiApplication>
#include <QPalette>
#include <QString>

namespace Utils {

namespace Internal {

class EnvironmentModelPrivate
{
public:
    void updateResultNameValueDictionary()
    {
        m_resultNameValueDictionary = m_baseNameValueDictionary;
        m_resultNameValueDictionary.modify(m_items);
        // Add removed variables again and mark them as "<UNSET>" so
        // that the user can actually see those removals:
        for (const EnvironmentItem &item : std::as_const(m_items)) {
            if (item.operation == EnvironmentItem::Unset)
                m_resultNameValueDictionary.set(item.name, Tr::tr("<UNSET>"));
        }
    }

    int findInChanges(const QString &name) const
    {
        const Qt::CaseSensitivity cs = m_baseNameValueDictionary.nameCaseSensitivity();

        const auto compare = [&name, &cs](const EnvironmentItem &item) {
            return item.name.compare(name, cs) == 0;
        };

        return Utils::indexOf(m_items, compare);
    }

    // Compares each key in dictionary against `key` and checks the result with `compare`.
    // Returns the index of the first key where the result of QString::compare() satisfies the
    // `compare` function.
    // Returns -1 if no such key is found.
    static int findIndex(
        const NameValueDictionary &dictionary, const QString &key, std::function<bool(int)> compare)
    {
        const Qt::CaseSensitivity cs = dictionary.nameCaseSensitivity();

        const auto compareFunc =
            [&key, cs, compare](const std::tuple<QString, QString, bool> &item) {
                return compare(std::get<0>(item).compare(key, cs));
            };

        return Utils::indexOf(dictionary, compareFunc);
    }

    int findInResultInsertPosition(const QString &key) const
    {
        const auto compare = [](int compareResult) { return compareResult > 0; };
        const int pos = findIndex(m_resultNameValueDictionary, key, compare);
        return pos >= 0 ? pos : m_resultNameValueDictionary.size();
    }

    int findInResult(const QString &key) const
    {
        const auto compare = [](int compareResult) { return compareResult == 0; };
        return findIndex(m_resultNameValueDictionary, key, compare);
    }

    NameValueDictionary m_baseNameValueDictionary;
    NameValueDictionary m_resultNameValueDictionary;
    EnvironmentItems m_items;
};

} // namespace Internal

EnvironmentModel::EnvironmentModel(QObject *parent)
    : QAbstractTableModel(parent)
    , d(std::make_unique<Internal::EnvironmentModelPrivate>())
{}

EnvironmentModel::~EnvironmentModel() = default;

QString EnvironmentModel::indexToVariable(const QModelIndex &index) const
{
    const auto it = std::next(d->m_resultNameValueDictionary.begin(), index.row());
    return it.key();
}

void EnvironmentModel::setBaseEnvironment(const Environment &env)
{
    const NameValueDictionary dictionary = env.toDictionary();
    if (d->m_baseNameValueDictionary == dictionary)
        return;
    beginResetModel();
    d->m_baseNameValueDictionary = dictionary;
    d->updateResultNameValueDictionary();
    endResetModel();
}

int EnvironmentModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d->m_resultNameValueDictionary.size();
}
int EnvironmentModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

bool EnvironmentModel::changes(const QString &name) const
{
    return d->findInChanges(name) >= 0;
}

Environment EnvironmentModel::baseEnvironment() const
{
    return Environment(d->m_baseNameValueDictionary);
}

QVariant EnvironmentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto it = std::next(d->m_resultNameValueDictionary.begin(), index.row());

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
        if (index.column() == 0)
            return it.key();
        if (index.column() == 1) {
            // Do not return "<UNSET>" when editing a previously unset variable:
            if (role == Qt::EditRole) {
                int pos = d->findInChanges(indexToVariable(index));
                if (pos != -1 && d->m_items.at(pos).operation == EnvironmentItem::Unset)
                    return QString();
            }
            QString value = it.value();
            if (role == Qt::ToolTipRole && value.length() > 80) {
                if (currentEntryIsPathList(index)) {
                    // For path lists, display one entry per line without separator
                    const QChar sep = Utils::HostOsInfo::pathListSeparator();
                    value = value.replace(sep, '\n');
                } else {
                    // Use html to enable text wrapping
                    value = value.toHtmlEscaped();
                    value.prepend(QLatin1String("<html><body>"));
                    value.append(QLatin1String("</body></html>"));
                }
            }
            return value;
        }
        break;
    case Qt::FontRole: {
        QFont f;
        f.setStrikeOut(!it.enabled());
        return f;
    }
    case Qt::ForegroundRole: {
        const QPalette p = QGuiApplication::palette();
        return p.color(changes(it.key()) ? QPalette::Link : QPalette::Text);
    }
    }
    return QVariant();
}

Qt::ItemFlags EnvironmentModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant EnvironmentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();
    return section == 0 ? Tr::tr("Variable") : Tr::tr("Value");
}

/// *****************
/// Utility functions
/// *****************
QModelIndex EnvironmentModel::variableToIndex(const QString &name) const
{
    int row = d->findInResult(name);
    if (row == -1)
        return QModelIndex();
    return index(row, 0);
}

bool EnvironmentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    // ignore changes to already set values:
    if (data(index, role) == value)
        return true;

    const QString oldName = data(this->index(index.row(), 0, QModelIndex())).toString();
    const QString oldValue = data(this->index(index.row(), 1, QModelIndex()), Qt::EditRole).toString();
    int changesPos = d->findInChanges(oldName);

    if (index.column() == 0) {
        //fail if a variable with the same name already exists
        const QString &newName = HostOsInfo::isWindowsHost() ? value.toString().toUpper()
                                                             : value.toString();
        if (newName.isEmpty() || newName.contains('='))
            return false;
        // Does the new name exist already?
        if (d->m_resultNameValueDictionary.hasKey(newName) || newName.isEmpty())
            return false;

        EnvironmentItem newVariable(newName, oldValue);

        if (changesPos != -1)
            resetVariable(oldName); // restore the original base variable again

        QModelIndex newIndex = addVariable(newVariable);      // add the new variable
        emit focusIndex(newIndex.sibling(newIndex.row(), 1)); // hint to focus on the value
        return true;
    } else if (index.column() == 1) {
        // We are changing an existing value:
        const QString stringValue = value.toString();
        if (changesPos != -1) {
            const auto oldIt = d->m_baseNameValueDictionary.find(oldName);
            const auto newIt = d->m_resultNameValueDictionary.find(oldName);
            // We have already changed this value
            if (oldIt != d->m_baseNameValueDictionary.end() && stringValue == oldIt.value()
                && oldIt.enabled() == newIt.enabled()) {
                // ... and now went back to the base value
                d->m_items.removeAt(changesPos);
            } else {
                // ... and changed it again
                d->m_items[changesPos].value = stringValue;
                if (d->m_items[changesPos].operation == EnvironmentItem::Unset)
                    d->m_items[changesPos].operation = EnvironmentItem::SetEnabled;
            }
        } else {
            // Add a new change item:
            d->m_items.append(EnvironmentItem(oldName, stringValue));
        }
        d->updateResultNameValueDictionary();
        emit dataChanged(index, index);
        emit userChangesChanged();
        return true;
    }
    return false;
}

QModelIndex EnvironmentModel::addVariable()
{
    return addVariable(EnvironmentItem("NEWVAR", "VALUE"));
}

QModelIndex EnvironmentModel::addVariable(const EnvironmentItem &item)
{
    // Return existing index if the name is already in the result set:
    int pos = d->findInResult(item.name);
    if (pos >= 0 && pos < d->m_resultNameValueDictionary.size())
        return index(pos, 0, QModelIndex());

    int insertPos = d->findInResultInsertPosition(item.name);
    int changePos = d->findInChanges(item.name);
    if (d->m_baseNameValueDictionary.hasKey(item.name)) {
        // We previously unset this!
        Q_ASSERT(changePos >= 0);
        // Do not insert a line here as we listed the variable as <UNSET> before!
        Q_ASSERT(d->m_items.at(changePos).name == item.name);
        Q_ASSERT(d->m_items.at(changePos).operation == EnvironmentItem::Unset);
        Q_ASSERT(d->m_items.at(changePos).value.isEmpty());
        d->m_items[changePos] = item;
        emit dataChanged(index(insertPos, 0, QModelIndex()), index(insertPos, 1, QModelIndex()));
    } else {
        // We add something that is not in the base dictionary
        // Insert a new line!
        QTC_ASSERT(insertPos >= 0, insertPos = d->m_resultNameValueDictionary.size());
        beginInsertRows(QModelIndex(), insertPos, insertPos);
        Q_ASSERT(changePos < 0);
        d->m_items.append(item);
        d->updateResultNameValueDictionary();
        endInsertRows();
    }
    emit userChangesChanged();
    return index(insertPos, 0, QModelIndex());
}

void EnvironmentModel::resetVariable(const QString &name)
{
    int rowInChanges = d->findInChanges(name);
    if (rowInChanges < 0)
        return;

    int rowInResult = d->findInResult(name);
    if (rowInResult < 0)
        return;

    if (d->m_baseNameValueDictionary.hasKey(name)) {
        d->m_items.removeAt(rowInChanges);
        d->updateResultNameValueDictionary();
        emit dataChanged(index(rowInResult, 0, QModelIndex()), index(rowInResult, 1, QModelIndex()));
        emit userChangesChanged();
    } else {
        // Remove the line completely!
        beginRemoveRows(QModelIndex(), rowInResult, rowInResult);
        d->m_items.removeAt(rowInChanges);
        d->updateResultNameValueDictionary();
        endRemoveRows();
        emit userChangesChanged();
    }
}

void EnvironmentModel::unsetVariable(const QString &name)
{
    // This does not change the number of rows as we will display a <UNSET>
    // in place of the original variable!
    int row = d->findInResult(name);
    if (row < 0)
        return;

    // look in d->m_items for the variable
    int pos = d->findInChanges(name);
    if (pos != -1) {
        d->m_items[pos].operation = EnvironmentItem::Unset;
        d->m_items[pos].value.clear();
        d->updateResultNameValueDictionary();
        emit dataChanged(index(row, 0, QModelIndex()), index(row, 1, QModelIndex()));
        emit userChangesChanged();
        return;
    }
    d->m_items.append(EnvironmentItem(name, QString(), EnvironmentItem::Unset));
    d->updateResultNameValueDictionary();
    emit dataChanged(index(row, 0, QModelIndex()), index(row, 1, QModelIndex()));
    emit userChangesChanged();
}

void EnvironmentModel::toggleVariable(const QModelIndex &idx)
{
    const QString name = indexToVariable(idx);
    const auto newIt = d->m_resultNameValueDictionary.find(name);
    QTC_ASSERT(newIt != d->m_resultNameValueDictionary.end(), return);
    const auto op = newIt.enabled() ? EnvironmentItem::SetDisabled : EnvironmentItem::SetEnabled;
    const int changesPos = d->findInChanges(name);
    if (changesPos != -1) {
        const auto oldIt = d->m_baseNameValueDictionary.find(name);
        if (oldIt == d->m_baseNameValueDictionary.end() || oldIt.value() != newIt.value()) {
            d->m_items[changesPos].operation = op;
        } else {
            d->m_items.removeAt(changesPos);
        }
    } else {
        d->m_items.append({name, newIt.value(), op});
    }
    d->updateResultNameValueDictionary();
    emit dataChanged(index(idx.row(), 0), index(idx.row(), 1));
    emit userChangesChanged();
}

bool EnvironmentModel::isUnset(const QString &name)
{
    const int pos = d->findInChanges(name);
    return pos == -1 ? false : d->m_items.at(pos).operation == EnvironmentItem::Unset;
}

bool EnvironmentModel::isEnabled(const QString &name) const
{
    return d->m_resultNameValueDictionary.find(name).enabled();
}

bool EnvironmentModel::canReset(const QString &name)
{
    return d->m_baseNameValueDictionary.hasKey(name);
}

EnvironmentItems EnvironmentModel::userChanges() const
{
    return d->m_items;
}

void EnvironmentModel::setUserChanges(const EnvironmentItems &items)
{
    EnvironmentItems filtered = Utils::filtered(items, [](const EnvironmentItem &i) {
        return i.operation == EnvironmentItem::Comment
                || (i.name != "export " && !i.name.contains('='));
    });
    // We assume nobody is reordering the items here.
    if (filtered == d->m_items)
        return;
    beginResetModel();
    d->m_items = filtered;
    for (EnvironmentItem &item : d->m_items) {
        QString &name = item.name;
        name = name.trimmed();
        if (name.startsWith("export "))
            name = name.mid(7).trimmed();
        if (d->m_baseNameValueDictionary.osType() == OsTypeWindows) {
            // NameValueDictionary variable names are case-insensitive under windows, but we still
            // want to preserve the case of pre-existing variables.
            auto it = d->m_baseNameValueDictionary.find(name);
            if (it != d->m_baseNameValueDictionary.end())
                name = it.key();
        }
    }

    d->updateResultNameValueDictionary();
    endResetModel();
    emit userChangesChanged();
}

bool EnvironmentModel::currentEntryIsPathList(const QModelIndex &current) const
{
    if (!current.isValid())
        return false;

    // Look at the name first and check it against some well-known path variables. Extend as needed.
    const QString varName = indexToVariable(current);
    if (varName.compare("PATH", Utils::HostOsInfo::fileNameCaseSensitivity()) == 0)
        return true;
    if (Utils::HostOsInfo::isMacHost()
        && (varName == "DYLD_LIBRARY_PATH" || varName == "DYLD_FRAMEWORK_PATH")) {
        return true;
    }
    if (Utils::HostOsInfo::isAnyUnixHost() && varName == "LD_LIBRARY_PATH")
        return true;
    if (varName == "PKG_CONFIG_DIR")
        return true;
    if (Utils::HostOsInfo::isWindowsHost()
            && QStringList{"INCLUDE", "LIB", "LIBPATH"}.contains(varName)) {
        return true;
    }

    // Now check the value: If it's a list of strings separated by the platform's path separator
    // and at least one of the strings is an existing directory, then that's enough proof for us.
    QModelIndex valueIndex = current;
    if (valueIndex.column() == 0)
        valueIndex = valueIndex.siblingAtColumn(1);
    const QStringList entries = data(valueIndex).toString()
            .split(Utils::HostOsInfo::pathListSeparator(), Qt::SkipEmptyParts);
    if (entries.length() < 2)
        return false;
    return Utils::anyOf(entries, [](const QString &d) { return QFileInfo(d).isDir(); });
}

} // namespace Utils
