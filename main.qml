import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.15
import NotebookCRUD 1.0

ApplicationWindow {
    id: window
    width: 720
    height: 560
    minimumWidth: 480
    minimumHeight: 400
    visible: true
    title: qsTr("Contacts Manager")

    ContactsModel {
        id: contactsData
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 8
            spacing: 12

            Label {
                text: "📇 " + qsTr("Contacts")
                font.pixelSize: 20
                font.bold: true
                Layout.alignment: Qt.AlignVCenter
            }

            Item { Layout.fillWidth: true }

            Label {
                text: contactsList.count + " " + qsTr("contact(s)")
                font.pixelSize: 13
                color: palette.mid
                Layout.alignment: Qt.AlignVCenter
            }

            ToolButton {
                text: "➕"
                font.pixelSize: 18
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Add new contact")
                onClicked: {
                    contactsData.appendEmptyRow()
                    contactsList.positionViewAtEnd()
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        ListExample {
            id: contactsList
            Layout.fillWidth: true
            Layout.fillHeight: true
            itemsModel: contactsData
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16

            Label {
                text: qsTr("Click on any field to edit • Swipe left to delete")
                font.pixelSize: 11
                color: palette.mid
                Layout.fillWidth: true
            }
        }
    }
}
