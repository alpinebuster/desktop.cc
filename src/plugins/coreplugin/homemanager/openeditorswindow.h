#pragma once

#include "documentmodel.h"
#include "homeview.h"

#include <QFrame>
#include <QIcon>
#include <QList>
#include <QTreeWidget>

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace Core {

class IEditor;

namespace Internal {

class OpenEditorsTreeWidget : public QTreeWidget {
public:
    explicit OpenEditorsTreeWidget(QWidget *parent = nullptr) : QTreeWidget(parent) {}
    ~OpenEditorsTreeWidget() override = default;
    QSize sizeHint() const override;
};


class OpenEditorsWindow : public QFrame
{
    Q_OBJECT

public:
    enum Mode {ListMode, HistoryMode };

    explicit OpenEditorsWindow(QWidget *parent = nullptr);

    void setEditors(const QList<EditLocation> &globalHistory, HomeView *view);

    bool eventFilter(QObject *src, QEvent *e) override;
    void focusInEvent(QFocusEvent*) override;
    void setVisible(bool visible) override;
    void selectNextEditor();
    void selectPreviousEditor();
    QSize sizeHint() const override;

public slots:
    void selectAndHide();

private:
    void editorClicked(QTreeWidgetItem *item);
    static void selectEditor(QTreeWidgetItem *item);

    void addHistoryItems(const QList<EditLocation> &history, HomeView *view,
                         QSet<const DocumentModel::Entry *> &entriesDone);
    void addRemainingItems(HomeView *view,
                           QSet<const DocumentModel::Entry *> &entriesDone);
    void addItem(DocumentModel::Entry *entry, QSet<const DocumentModel::Entry *> &entriesDone,
                 HomeView *view);
    void ensureCurrentVisible();
    void selectUpDown(bool up);

    bool isSameFile(IEditor *editorA, IEditor *editorB) const;

    const QIcon m_emptyIcon;
    OpenEditorsTreeWidget *m_editorList;
};

} // namespace Internal
} // namespace Core
