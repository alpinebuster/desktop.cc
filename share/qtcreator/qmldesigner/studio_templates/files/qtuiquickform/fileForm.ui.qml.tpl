/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/

import QtQuick
@if %{UseQtQuickControls2}
import QtQuick.Controls
@endif
@if %{UseImport}
import %{ApplicationImport}
@endif

%{RootItem} {
@if %{UseImport}
    width: Constants.width
    height: Constants.height
@else
    width: 1024
    height: 768
@endif

    property alias button: button

    Button {
        id: button
        x: 64
        y: 64
        text: qsTr("Button")
    }
}
