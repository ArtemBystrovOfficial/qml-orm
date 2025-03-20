import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Rectangle {
    property var taskItem
    property string fullText
    id: root
    function  saveChanges() {
        taskItem.title = titleArea.text
        taskItem.desc = descArea.text
        taskItem.isBusy = false
    }
    
    color:"#444444"

    ColumnLayout {
        anchors.fill:parent
        spacing:0
        TextArea {
            id: titleArea
            text: taskItem ? taskItem.title : ""
            color: "white"
            font.family: "Helvetica"
            font.pointSize: 12
            onTextChanged: {
                if (text.length > 50)
                    text = text.slice(0, -1); 
            }
            Layout.leftMargin: 10
            width: 200
            height: 30
        }
        Rectangle {
            color: "silver"
            Layout.leftMargin: 10
            width: 200
            height: 1
        }
        TextArea {
            id: descArea
            text: fullText
            wrapMode: Text.Wrap
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "white"
            font.family: "Helvetica"
            font.pointSize: 9
        }
    }

}