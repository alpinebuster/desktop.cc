#pragma once

#include "utils_global.h"

#include <QVariant>
#include <QWidget>

namespace Utils {

class CrumblePathButton;

class QTCREATOR_UTILS_EXPORT CrumblePath : public QWidget
{
    Q_OBJECT

public:
    explicit CrumblePath(QWidget *parent = nullptr);
    ~CrumblePath() override;

    QVariant dataForIndex(int index) const;
    QVariant dataForLastIndex() const;
    int length() const;

public slots:
    void pushElement(const QString &title, const QVariant &data = QVariant());
    void addChild(const QString &title, const QVariant &data = QVariant());
    void popElement();
    virtual void clear();

signals:
    void elementClicked(const QVariant &data);

private:
    void emitElementClicked();
    void setBackgroundStyle();

private:
    QList<CrumblePathButton*> m_buttons;
    QLayout *m_buttonsLayout = nullptr;
};

} // namespace Utils
