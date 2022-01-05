#include "categorysortfiltermodel.h"

#include <QRegularExpression>

namespace Utils {

CategorySortFilterModel::CategorySortFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool CategorySortFilterModel::filterAcceptsRow(int source_row,
                                               const QModelIndex &source_parent) const
{
    if (!source_parent.isValid()) {
        // category items should be visible if any of its children match
        const QRegularExpression &regexp = filterRegularExpression();
        const QModelIndex &categoryIndex = sourceModel()->index(source_row, 0, source_parent);
        if (regexp.match(sourceModel()->data(categoryIndex, filterRole()).toString()).hasMatch())
            return true;
        const int rowCount = sourceModel()->rowCount(categoryIndex);
        for (int row = 0; row < rowCount; ++row) {
            if (filterAcceptsRow(row, categoryIndex))
                return true;
        }
        return false;
    }
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

} // Utils

