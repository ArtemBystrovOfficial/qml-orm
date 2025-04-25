import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import CppObjects 1.0

Rectangle {
    id: root
    color:"#222222"
    property int tasksMargin: 30
    property int taskHeight: 30
    
    signal openedTask(int index)
    signal deletedTask(int index)

    function addEmptyTask() {
        TaskModel.addEmptyTask();
    }

    ScrollView {
        anchors.fill: parent
        contentHeight: taskContainer.count * (taskHeight + mainColumn.spacing) + tasksMargin
        Item {
            anchors.fill: parent
            Column {
                id: mainColumn
                width: parent.width - tasksMargin
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: tasksMargin / 2
                anchors.leftMargin: tasksMargin / 2
                spacing: 2
                Repeater {
                    id: taskContainer
                    model: TaskModel
                    TaskHeader {
                        width: parent.width
                        height: taskHeight
                        color_scheme: {
                            return item.color_scheme.color
                        } 
                        header_title: {
                            var title = item.title
                            return !title.length ? qsTr("Без имени") : title 
                        }
                        header_updated_at: item.updatedAt
                        header_desc: item.desc
                        is_busy: false //item.isBusy

                        onOpened: openTask(index)    
                        onDeleted: deletedTask(index)
                        onSetRandColor: {
						    var hex = Math.floor(Math.random() * 0x1000000)
								.toString(16)
								.padStart(6, "0");
							item.color_scheme.color = "#" + hex
							//item.replaceByIdColorScheme(3)
							//item.callNestedSignal()
							TaskModel.CommitChanges() 
							//TaskModel.replaceByIdColorScheme(5)
                            //item.colorSchemeId = ColorSchemeModel.itemAt(
                            //    Math.floor(Math.random()*ColorSchemeModel.rowCount()
                            //)).id
                            //TaskModel.CommitChanges()
                        }
                    }
                }
            }
        }
    }
}