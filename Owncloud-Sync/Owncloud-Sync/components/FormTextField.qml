import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: root

    property alias title: _title
    property alias textfield: _textfield

    width: parent.width
    height: _title.implicitHeight + _textfield.height + _textfield.anchors.topMargin

    Label {
        id:_title
        anchors { left: parent.left; top: parent.top; right: parent.right }
    }
    
    TextField {
        id: _textfield
        height: units.gu(4)
        anchors { top: _title.bottom; left: parent.left; right: parent.right; topMargin: units.gu(0.5) }
    }
}
