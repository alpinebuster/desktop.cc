#include "helloworldwindow.h"

#include <QTextEdit>
#include <QVBoxLayout>

using namespace HelloWorld::Internal;

HelloWorldWindow::HelloWorldWindow(QWidget *parent)
   : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->addWidget(new QTextEdit(tr("Focus me to activate my context!")));
    setWindowTitle(tr("Hello, world!"));
}
