#pragma once

#include "core_global.h"

#include <utils/id.h>
#include <utils/porting.h>

#include <QWidget>
#include <QObject>

#include <functional>

QT_BEGIN_NAMESPACE
class QPixmap;
QT_END_NAMESPACE

namespace Core {

class CORE_EXPORT IWelcomePage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(int priority READ priority CONSTANT)

public:
    IWelcomePage();
    ~IWelcomePage() override;

    virtual QString title() const = 0;
    virtual int priority() const { return 0; }
    virtual Utils::Id id() const = 0;
    virtual QWidget *createWidget() const = 0;

    static const QList<IWelcomePage *> allWelcomePages();
};

class WelcomePageButtonPrivate;

class CORE_EXPORT WelcomePageFrame : public QWidget
{
public:
    WelcomePageFrame(QWidget *parent);

    void paintEvent(QPaintEvent *event) override;

    static QPalette buttonPalette(bool isActive, bool isCursorInside, bool forText);
};

class CORE_EXPORT WelcomePageButton : public WelcomePageFrame
{
public:
    WelcomePageButton(QWidget *parent);
    ~WelcomePageButton() override;

    void mousePressEvent(QMouseEvent *) override;
    void enterEvent(Utils::EnterEvent *) override;
    void leaveEvent(QEvent *) override;

    void setText(const QString &text);
    void setIcon(const QPixmap &pixmap);
    void setOnClicked(const std::function<void ()> &value);
    void setActiveChecker(const std::function<bool ()> &value);
    void recheckActive();
    void click();

private:
    WelcomePageButtonPrivate *d;
};

} // Core
