#pragma once

#include "utils_global.h"

#include <QWidget>

#include <functional>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

namespace Utils {

struct PathListEditorPrivate;

class QTCREATOR_UTILS_EXPORT PathListEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QStringList pathList READ pathList WRITE setPathList DESIGNABLE true)
    Q_PROPERTY(QString fileDialogTitle READ fileDialogTitle WRITE setFileDialogTitle DESIGNABLE true)

public:
    explicit PathListEditor(QWidget *parent = nullptr);
    ~PathListEditor() override;

    QString pathListString() const;
    QStringList pathList() const;
    QString fileDialogTitle() const;

    void clear();
    void setPathList(const QStringList &l);
    void setPathList(const QString &pathString);
    void setFileDialogTitle(const QString &l);

protected:
    // Index after which to insert further "Add" buttons
    static const int lastInsertButtonIndex;

    QPushButton *addButton(const QString &text, QObject *parent, std::function<void()> slotFunc);
    QPushButton *insertButton(int index /* -1 */, const QString &text, QObject *parent,
                              std::function<void()> slotFunc);

    QString text() const;
    void setText(const QString &);

    void insertPathAtCursor(const QString &);
    void deletePathAtCursor();

private:
    PathListEditorPrivate *d;
};

} // namespace Utils
