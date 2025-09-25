import QtQuick 2.15

GridView {
    width: 200
    height: 300

    delegate: Column {
        Text {
            text: name
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Text {
            text: email
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
