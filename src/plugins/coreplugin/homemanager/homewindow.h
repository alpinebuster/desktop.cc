#pragma once

#include <QWidget>

namespace Core {
namespace Internal {

class HomeArea;

class HomeWindow : public QWidget
{
    Q_OBJECT
public:
    explicit HomeWindow(QWidget *parent = nullptr);
    ~HomeWindow() override;

    HomeArea *editorArea() const;

    QVariantHash saveState() const;
    void restoreState(const QVariantHash &state);
private:
    void updateWindowTitle();

    HomeArea *m_area;
};

} // Internal
} // Core
