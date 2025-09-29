import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ListView {
    id: list
    width: 300
    height: 200
    spacing: 5
    model: contactsModel

    // пример представления элемента в списке, кстати, Frame - шаблон отображения, его можно заменить на другой элемент
    delegate: Frame {
        width: list.width
        implicitHeight: 48   // высота делегата
        padding: 6

        RowLayout {
            anchors.fill: parent
            spacing: 8

            // Главная секция — занимает всё доступное место
            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                spacing: 6

                // Имя — фиксируем минимальную/предпочтительную ширину
                EditableText {
                    field: name
                    Layout.preferredWidth: 120
                    Layout.minimumWidth: 80
                    Layout.alignment: Qt.AlignVCenter
                    // Внутри вашего EditableText можно включить elide/clip,
                    // чтобы текст не ломал разметку.
                }

                Text {
                    text: ":"
                    font.pointSize: 16
                    Layout.alignment: Qt.AlignVCenter
                }

                // Email тянется и занимает оставшееся пространство
                EditableText {
                    field: email
                    Layout.fillWidth: true
                    Layout.preferredWidth: 220
                    Layout.minimumWidth: 120
                    Layout.alignment: Qt.AlignVCenter
                    // если ваш EditableText использует Text — добавьте elide: Text.ElideRight
                }
            }

            // Кнопка удаления — фиксированный аккуратный размер
            Button {
                text: "-"
                font.pointSize: 16
                Layout.preferredWidth: 32
                Layout.maximumWidth: 36
                Layout.preferredHeight: 32
                Layout.alignment: Qt.AlignVCenter
                onClicked: list.model.removeAt(model.row)
            }
        }
    }

    header: Rectangle {
        width: parent.width
        height: 30
        color: "red"
        Text {
            text: "Contacts list"
            color: "white"
            font.pointSize: 18
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    // z - указ
    footer: Rectangle {
        width: parent.width
        height: 20
        color: "red"
        z: 10
    }
    footerPositioning: ListView.OverlayFooter
}
