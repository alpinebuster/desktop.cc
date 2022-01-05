#include "messagebox.h"

#include <QMessageBox>

#include "icore.h"

namespace Core {
namespace AsynchronousMessageBox {

namespace {

QWidget *message(QMessageBox::Icon icon, const QString &title, const QString &desciption)
{
    QMessageBox *messageBox = new QMessageBox(icon,
                                              title,
                                              desciption,
                                              QMessageBox::Ok,
                                              Core::ICore::dialogParent());

    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->setModal(true);
    messageBox->show();
    return messageBox;
}
}

QWidget *warning(const QString &title, const QString &desciption)
{
    return message(QMessageBox::Warning, title, desciption);
}

QWidget *information(const QString &title, const QString &desciption)
{
    return message(QMessageBox::Information, title, desciption);
}

QWidget *critical(const QString &title, const QString &desciption)
{
    return message(QMessageBox::Critical, title, desciption);
}

}
}
