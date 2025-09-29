import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

RowLayout {
    id: root
    property alias field: displayField.text   // двунаправленной привязки здесь нет — смотрим только отображение
    // для логов можно задать optional property int row: -1

    // Text отображается, когда не редактируем
    Text {
        id: displayField
        text: ""
        font.pointSize: 16
        visible: !editField.visible && text.length > 0

        MouseArea {
            anchors.fill: parent
            onClicked: {
                editField.visible = true
                editField.forceActiveFocus()
            }
        }
    }

    // TextField для редактирования
    TextField {
        id: editField
        visible: false
        text: displayField.text

        // при окончании редактирования обновляем поле (и делаем кнопку видимой/невидимой)
        onEditingFinished: {
            if (text.length !== 0) {
                // обновляем текст, который отображается
                displayField.text = text
            }
            editField.visible = false
        }
    }

    Component.onCompleted: {
        // Лог один раз при создании. Если нужно отслеживать изменения — добавь onTextChanged ниже.
        console.log("EditableText created, field =", displayField.text)
    }

    // Лог при изменении поля (удалить/закомментировать в релизе)
    onFieldChanged: {
        console.log("EditableText.field changed ->", displayField.text)
    }
}


// RowLayout {
//     required property string field;

//     Text {
//         id: nameText
//         text: field
//         font.pointSize: 16
//         visible: field.length !== 0

//         MouseArea {
//             anchors.fill: nameText
//             onClicked: {
//                 nameText.visible = false;
//                 nameEditField.visible = true;
//             }
//         }
//     }
//     TextField {
//         id: nameEditField
//         visible: field.length === 0
//         text: field

//         onEditingFinished: {
//             if (text.length !== 0) {
//                 field = text
//                 nameText.visible = true;
//                 nameEditField.visible = false;
//             }
//         }
//     }
// }
