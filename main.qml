import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.3
import NotebookCRUD 1.0

ApplicationWindow {
    width: 800
    height: 600
    visible: true
    title: qsTr("Contacts Manager")

    ContactsModel {
        id: contactsData
        
        onValidationError: function(message) {
            errorDialog.text = message
            errorDialog.open()
        }
        
        onImportCompleted: function(count) {
            statusBar.text = qsTr("Imported %1 contacts successfully").arg(count)
            statusTimer.restart()
        }
        
        onExportCompleted: function(success) {
            if (success) {
                statusBar.text = qsTr("Contacts exported successfully")
            } else {
                statusBar.text = qsTr("Export failed: %1").arg(contactsData.lastError)
            }
            statusTimer.restart()
        }
    }

    PdfConverter {
        id: pdfConverter
        
        onConversionCompleted: function(success, message) {
            if (success) {
                statusBar.text = qsTr("PDF created successfully")
                pdfResultDialog.title = qsTr("Success")
                pdfResultDialog.icon = StandardIcon.Information
            } else {
                statusBar.text = qsTr("PDF conversion failed")
                pdfResultDialog.title = qsTr("Error")
                pdfResultDialog.icon = StandardIcon.Critical
            }
            pdfResultDialog.text = message
            pdfResultDialog.open()
            statusTimer.restart()
        }
    }

    // Диалог ошибок
    MessageDialog {
        id: errorDialog
        title: qsTr("Validation Error")
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Ok
    }

    // Диалог результата конвертации PDF
    MessageDialog {
        id: pdfResultDialog
        title: qsTr("PDF Conversion")
        standardButtons: StandardButton.Ok
    }

    // Диалог выбора Markdown файла
    FileDialog {
        id: mdFileDialog
        title: qsTr("Select Markdown file to convert")
        selectExisting: true
        nameFilters: ["Markdown files (*.md)", "All files (*)"]
        onAccepted: {
            pdfSaveDialog.open()
        }
    }

    // Диалог сохранения PDF
    FileDialog {
        id: pdfSaveDialog
        title: qsTr("Save PDF as")
        selectExisting: false
        nameFilters: ["PDF files (*.pdf)"]
        defaultSuffix: "pdf"
        onAccepted: {
            pdfConverter.convertMarkdownToPdf(
                mdFileDialog.fileUrl.toString(),
                pdfSaveDialog.fileUrl.toString()
            )
        }
    }

    // Диалог экспорта
    FileDialog {
        id: exportDialog
        title: qsTr("Export Contacts to JSON")
        selectExisting: false
        nameFilters: ["JSON files (*.json)"]
        defaultSuffix: "json"
        onAccepted: {
            var path = exportDialog.fileUrl.toString().replace("file:///", "")
            contactsData.exportToJson(path)
        }
    }

    // Диалог импорта из API
    Dialog {
        id: importDialog
        title: qsTr("Import from API")
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        width: 400
        
        ColumnLayout {
            width: parent.width
            spacing: 8
            
            Label {
                text: qsTr("Enter API URL (must return JSON array):")
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            
            TextField {
                id: apiUrlField
                placeholderText: "https://api.example.com/contacts"
                Layout.fillWidth: true
                text: "https://jsonplaceholder.typicode.com/users"
            }
            
            Label {
                text: qsTr("Expected format: [{\"name\": \"...\", \"email\": \"...\"}]")
                font.pixelSize: 10
                color: "#666"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }
        
        onAccepted: {
            if (apiUrlField.text.length > 0) {
                contactsData.importFromJsonApi(apiUrlField.text)
            }
        }
    }

    header: ToolBar {
        ColumnLayout {
            anchors.fill: parent
            spacing: 4
            
            // Верхняя строка с заголовком и кнопками
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                
                Label {
                    text: qsTr("Contacts")
                    font.pixelSize: 18
                    font.bold: true
                    Layout.alignment: Qt.AlignVCenter
                }
                
                Label {
                    text: qsTr("(%1 total)").arg(contactsData.count)
                    font.pixelSize: 12
                    color: "#666"
                    Layout.alignment: Qt.AlignVCenter
                }
                
                Item { Layout.fillWidth: true }
                
                ToolButton {
                    text: qsTr("Import from API")
                    icon.name: "download"
                    Accessible.name: qsTr("Import contacts from API")
                    onClicked: importDialog.open()
                    enabled: !contactsData.isLoading
                }
                
                ToolButton {
                    text: qsTr("MD to PDF")
                    icon.name: "document-export"
                    Accessible.name: qsTr("Convert Markdown file to PDF")
                    onClicked: mdFileDialog.open()
                    enabled: !pdfConverter.isConverting
                }
                
                ToolButton {
                    text: qsTr("Export JSON")
                    icon.name: "document-save"
                    Accessible.name: qsTr("Export contacts to JSON")
                    onClicked: exportDialog.open()
                    enabled: contactsData.count > 0
                }
                
                ToolButton {
                    text: "+"
                    font.pixelSize: 20
                    Accessible.name: qsTr("Add contact")
                    onClicked: contactsData.appendEmptyRow()
                }
            }
            
            // Строка поиска
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                
                TextField {
                    id: searchField
                    placeholderText: qsTr("Search contacts...")
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    
                    onTextChanged: {
                        contactsData.filterString = text
                    }
                    
                    // Иконка поиска
                    leftPadding: 30
                    
                    Label {
                        text: "🔍"
                        anchors.left: parent.left
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 16
                    }
                    
                    // Кнопка очистки
                    Button {
                        text: "✕"
                        flat: true
                        visible: searchField.text.length > 0
                        anchors.right: parent.right
                        anchors.rightMargin: 4
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24
                        height: 24
                        
                        onClicked: searchField.text = ""
                    }
                }
            }
        }
    }

    // Основной контент
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // Индикатор загрузки
        BusyIndicator {
            running: contactsData.isLoading
            Layout.alignment: Qt.AlignHCenter
            visible: running
        }

        // Список контактов
        ListExample {
            id: contactsList
            Layout.fillWidth: true
            Layout.fillHeight: true
            itemsModel: contactsData
            visible: !contactsData.isLoading
        }
    }

    // Статус-бар
    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            
            Label {
                id: statusBar
                text: qsTr("Ready")
                Layout.fillWidth: true
                font.pixelSize: 11
                
                Timer {
                    id: statusTimer
                    interval: 5000
                    onTriggered: statusBar.text = qsTr("Ready")
                }
            }
            
            Label {
                text: contactsData.lastError
                visible: contactsData.lastError.length > 0
                color: "#d32f2f"
                font.pixelSize: 11
            }
        }
    }
}
