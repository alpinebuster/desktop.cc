#pragma once

#include <QWidget>

namespace HelloWorld {
namespace Internal {

class HelloWorldWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HelloWorldWindow(QWidget *parent = nullptr);
};

} // namespace Internal
} // namespace HelloWorld
