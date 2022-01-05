#pragma once

#include "../iwizardfactory.h"

#include <QDialog>
#include <QIcon>
#include <QList>
#include <QVariantMap>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QSortFilterProxyModel;
class QPushButton;
class QStandardItem;
class QStandardItemModel;
QT_END_NAMESPACE

namespace Core {

namespace Internal {

namespace Ui { class NewDialog; }

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDialog(QWidget *parent);
    ~NewDialog() override;

    void setWizardFactories(QList<IWizardFactory*> factories, const QString &defaultLocation, const QVariantMap &extraVariables);

    void showDialog();

    static QWidget *currentDialog();

protected:
    bool event(QEvent *) override;

private:
    void currentCategoryChanged(const QModelIndex &);
    void currentItemChanged(const QModelIndex &);
    void accept() override;
    void reject() override;
    void updateOkButton();

    Core::IWizardFactory *currentWizardFactory() const;
    void addItem(QStandardItem *topLevelCategoryItem, IWizardFactory *factory);
    void saveState();

    static QWidget *m_currentDialog;

    Ui::NewDialog *m_ui;
    QStandardItemModel *m_model;
    QSortFilterProxyModel *m_filterProxyModel;
    QPushButton *m_okButton = nullptr;
    QIcon m_dummyIcon;
    QList<QStandardItem*> m_categoryItems;
    QString m_defaultLocation;
    QVariantMap m_extraVariables;
};

} // namespace Internal
} // namespace Core
