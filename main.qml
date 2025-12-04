import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import NotebookCRUD 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Contacts")

    ContactsModel {
        id: contactsData
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 8

            Label {
                text: qsTr("Contacts")
                font.pixelSize: 18
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                text: "+"
                Accessible.name: qsTr("Add contact")
                onClicked: contactsData.appendEmptyRow()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        ListExample {
            id: contactsList
            Layout.fillWidth: true
            Layout.fillHeight: true
            itemsModel: contactsData
        }
    }
}
