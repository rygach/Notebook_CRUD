import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

RowLayout {
    required property string field;

    Text {
        id: nameText
        text: field
        font.pointSize: 16
        visible: field.length !== 0

        MouseArea {
            anchors.fill: nameText
            onClicked: {
                nameText.visible = false;
                nameEditField.visible = true;
            }
        }
    }
    TextField {
        id: nameEditField
        visible: field.length === 0
        text: field

        onEditingFinished: {
            if (text.length !== 0) {
                field = text
                nameText.visible = true;
                nameEditField.visible = false;
            }
        }
    }
}
