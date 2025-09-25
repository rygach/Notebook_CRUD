import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import NotebookCRUD 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    ContactsModel {
        id: contactsData
    }

    ColumnLayout {
        ListExample {
            model: contactsData
        }

        Button {
            text: "Add"
            onClicked: contactsData.appendEmptyRow()
        }
    }
}
