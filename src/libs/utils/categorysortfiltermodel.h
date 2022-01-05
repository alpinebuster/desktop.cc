#pragma once

#include "utils_global.h"

#include <QSortFilterProxyModel>

namespace Utils {

class QTCREATOR_UTILS_EXPORT CategorySortFilterModel : public QSortFilterProxyModel
{
public:
    CategorySortFilterModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

} // Utils
