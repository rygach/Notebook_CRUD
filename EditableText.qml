import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

RowLayout {
    id: root
    // Публичные свойства для двунаправленной работы
    property string field: ""
    property bool editing: false
    signal committed(string value)

    // Text отображается, когда не редактируем
    Text {
        id: displayField
        text: root.field
        font.pointSize: 16
        visible: !root.editing && text.length > 0

        MouseArea {
            anchors.fill: parent
            onClicked: { root.editing = true; editField.forceActiveFocus() }
        }
    }

    // TextField для редактирования
    TextField {
        id: editField
        visible: root.editing
        text: root.field
        selectByMouse: true
        focus: visible

        // при окончании редактирования обновляем поле (и делаем кнопку видимой/невидимой)
        Keys.onReturnPressed: { commitChange() }
        Keys.onEnterPressed: { commitChange() }
        Keys.onEscapePressed: { root.editing = false }
        onEditingFinished: commitChange()
    }

    function commitChange() {
        if (editField.text !== root.field) {
            root.field = editField.text
            root.committed(root.field)
        }
        root.editing = false
    }

    Component.onCompleted: console.log("EditableText created, field =", root.field)

    // Лог при изменении поля (удалить/закомментировать в релизе)
    onFieldChanged: console.log("EditableText.field changed ->", root.field)
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
