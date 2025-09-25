import QtQuick 2.15
import QtQuick.Controls 2.0

ListView {
    id: list
    width: 300
    height: 200
    spacing: 5

    // пример представления элемента в списке, кстати, Frame - шаблон отображения, его можно заменить на другой элемент
    delegate: Frame {
        width: list.width
        Text {
            text: name + ": " + email
            font.pointSize: 16
            anchors.centerIn: parent
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
