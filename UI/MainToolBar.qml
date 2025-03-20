import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3   
ToolBar {
    id: root 
    property var mainStackView
    signal newTask()
    signal searchRequest(string text)
    signal returnFromTask()
    width:480
    RowLayout {
        anchors.fill: parent
        ToolButton {
            enabled: mainStackView.depth > 1
            text: qsTr("‹")
            font.pixelSize: 20
            onClicked: returnFromTask()  
        }
        Label {
            text: mainStackView.depth < 2 ? qsTr("Задачи") : qsTr("Редактор")
            elide: Label.ElideRight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }
        TextField {
            Layout.fillWidth: true
            visible: mainStackView.depth < 2
            onTextEdited: {
                searchRequest(text)
            }
        }
        ToolButton {
            visible: mainStackView.depth < 2
            text: qsTr("+")
            onClicked: newTask()
        }
    }
}