import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ListView {
    id: list
    required property var itemsModel
    clip: true
    spacing: 1
    Layout.fillWidth: true
    Layout.fillHeight: true
    model: itemsModel
    
    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
    }

    delegate: SwipeDelegate {
        id: rowDelegate
        width: list.width
        implicitHeight: 64
        padding: 12
        
        // Локальные свойства для реактивности
        property string contactName: model.name
        property string contactEmail: model.email
        
        background: Rectangle {
            color: rowDelegate.pressed ? Qt.darker(palette.base, 1.05) : 
                   rowDelegate.hovered ? Qt.lighter(palette.base, 1.02) : 
                   palette.base
            
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: palette.mid
                opacity: 0.3
            }
        }

        contentItem: RowLayout {
            spacing: 16

            // Иконка контакта
            Rectangle {
                width: 44
                height: 44
                radius: 22
                color: Qt.rgba(0.2, 0.6, 0.9, 0.15)
                border.color: Qt.rgba(0.2, 0.6, 0.9, 0.3)
                border.width: 1
                Layout.alignment: Qt.AlignVCenter
                
                Label {
                    anchors.centerIn: parent
                    text: rowDelegate.contactName.length > 0 ? rowDelegate.contactName.charAt(0).toUpperCase() : "?"
                    font.pixelSize: 20
                    font.bold: true
                    color: Qt.rgba(0.2, 0.6, 0.9, 1.0)
                }
            }

            // Поля контакта
            ColumnLayout {
                Layout.fillWidth: true
                Layout.minimumWidth: 200
                Layout.alignment: Qt.AlignVCenter
                spacing: 4

                EditableText {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 22
                    field: rowDelegate.contactName
                    placeholderText: qsTr("Name")
                    onCommitted: function(value) {
                        list.model.setData(list.model.index(model.row, 0), value, 256)
                        rowDelegate.contactName = value
                    }
                }

                EditableText {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 22
                    field: rowDelegate.contactEmail
                    placeholderText: qsTr("Email")
                    onCommitted: function(value) {
                        list.model.setData(list.model.index(model.row, 0), value, 257)
                        rowDelegate.contactEmail = value
                    }
                }
            }
        }

        swipe.right: Rectangle {
            width: 80
            height: parent.height
            anchors.right: parent.right
            color: "#e74c3c"

            Column {
                anchors.centerIn: parent
                spacing: 4
                
                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "🗑️"
                    font.pixelSize: 24
                }
                
                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Delete")
                    font.pixelSize: 11
                    color: "white"
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: list.model.removeAt(model.row)
            }
        }
    }

    // Пустое состояние
    Label {
        anchors.centerIn: parent
        visible: list.count === 0
        text: qsTr("No contacts yet\nClick '+' to add your first contact")
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 16
        color: palette.placeholderText
        lineHeight: 1.5
    }
}
