#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QEvent;
QT_END_NAMESPACE

namespace Core {
namespace Internal {

class VersionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VersionDialog(QWidget *parent);

    bool event(QEvent *event) override;

};

} // namespace Internal
} // namespace Core
