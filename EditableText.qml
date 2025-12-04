import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: root
    property string field: ""
    property string placeholderText: qsTr("Enter text...")
    property bool editing: false
    signal committed(string value)
    
    Layout.fillWidth: true
    implicitHeight: 26

    // Text отображается когда не редактируем
    Label {
        id: displayField
        anchors.fill: parent
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        text: root.field.length > 0 ? root.field : root.placeholderText
        font.pixelSize: 14
        color: root.field.length > 0 ? palette.text : palette.placeholderText
        visible: !root.editing
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        elide: Text.ElideRight
        
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            
            onEntered: parent.font.underline = true
            onExited: parent.font.underline = false
            onClicked: {
                root.editing = true
                editField.forceActiveFocus()
                editField.selectAll()
            }
        }
    }

    // TextField для редактирования
    TextField {
        id: editField
        anchors.fill: parent
        visible: root.editing
        text: root.field
        placeholderText: root.placeholderText
        selectByMouse: true
        focus: visible
        font.pixelSize: 14
        leftPadding: 4
        rightPadding: 4
        topPadding: 0
        bottomPadding: 0
        verticalAlignment: TextInput.AlignVCenter
        
        background: Rectangle {
            color: "transparent"
            border.color: palette.highlight
            border.width: editField.activeFocus ? 2 : 1
            radius: 2
        }
        
        Keys.onReturnPressed: commitChange()
        Keys.onEnterPressed: commitChange()
        Keys.onEscapePressed: {
            text = root.field
            root.editing = false
        }
        onEditingFinished: commitChange()
    }

    function commitChange() {
        if (editField.text !== root.field) {
            root.field = editField.text
            root.committed(root.field)
        }
        root.editing = false
    }
}
