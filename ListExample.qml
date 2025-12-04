import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ListView {
    id: list
    // используем отдельное имя, чтобы не конфликтовать с ListView.model
    required property var itemsModel
    clip: true
    spacing: 6
    Layout.fillWidth: true
    Layout.fillHeight: true
    model: itemsModel

    // пример представления элемента в списке, кстати, Frame - шаблон отображения, его можно заменить на другой элемент
    delegate: SwipeDelegate {
        id: rowDelegate
        width: list.width
        implicitHeight: 48
        padding: 6

        contentItem: RowLayout {
            spacing: 8

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                spacing: 6

                EditableText {
                    field: model.name
                    Layout.preferredWidth: 140
                    Layout.minimumWidth: 100
                    Layout.alignment: Qt.AlignVCenter
                    onCommitted: function(value) {
                        list.model.setData(list.model.index(model.row, 0), value, 256) // NameRole = 256
                    }
                }

                Text {
                    text: ":"
                    font.pixelSize: 16
                    Layout.alignment: Qt.AlignVCenter
                }

                EditableText {
                    field: model.email
                    Layout.fillWidth: true
                    Layout.preferredWidth: 240
                    Layout.minimumWidth: 140
                    Layout.alignment: Qt.AlignVCenter
                    onCommitted: function(value) {
                        list.model.setData(list.model.index(model.row, 0), value, 257) // EmailRole = 257
                    }
                }
            }
        }

        swipe.right: Rectangle {
            width: parent.height
            height: parent.height
            anchors.right: parent.right
            color: "tomato"

            ToolButton {
                anchors.fill: parent
                text: qsTr("Delete")
                Accessible.name: qsTr("Delete contact")
                onClicked: {
                    list.model.removeAt(model.row)
                }
            }
        }
    }

    header: Loader { active: false }

    footer: Item {
        width: parent.width
        height: model && model.count === 0 ? 120 : 0

        Column {
            anchors.centerIn: parent
            spacing: 8

            Label { text: qsTr("No contacts yet") }
            Button {
                text: qsTr("Add contact")
                onClicked: list.model.appendEmptyRow()
            }
        }
    }
    footerPositioning: ListView.OverlayFooter
}
