#pragma once

#include "utils_global.h"

#include "itemviews.h"

namespace Utils {

class QTCREATOR_UTILS_EXPORT NavigationTreeView : public TreeView
{
    Q_OBJECT
public:
    explicit NavigationTreeView(QWidget *parent = nullptr);
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
};

} // Utils
