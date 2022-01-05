#pragma once

#include <QList>
#include <QDialog>

#include "ui_saveitemsdialog.h"

namespace Core {

class IDocument;

namespace Internal {

class SaveItemsDialog : public QDialog
{
    Q_OBJECT

public:
    SaveItemsDialog(QWidget *parent, QList<IDocument *> items);

    void setMessage(const QString &msg);
    void setAlwaysSaveMessage(const QString &msg);
    bool alwaysSaveChecked();
    QList<IDocument *> itemsToSave() const;
    QStringList filesToDiff() const;

private:
    void collectItemsToSave();
    void collectFilesToDiff();
    void discardAll();
    void updateButtons();
    void adjustButtonWidths();

    Ui::SaveItemsDialog m_ui;
    QList<IDocument*> m_itemsToSave;
    QStringList m_filesToDiff;
    QPushButton *m_diffButton = nullptr;
};

} // namespace Internal
} // namespace Core
