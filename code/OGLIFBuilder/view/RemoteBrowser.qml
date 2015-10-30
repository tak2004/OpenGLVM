import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2
import QtQml.Models 2.1
import "Components"

Item{
    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    SplitView{
        id: splitView2
        anchors.fill: parent
        orientation: Qt.Horizontal

        ColumnLayout {
            id: functions
            anchors.bottomMargin: 5
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 5
            Layout.minimumWidth: 200
            Layout.maximumWidth: 400
            anchors.margins: 3

            Label{
                text: "Functions"
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.top: parent.top
                anchors.topMargin: 0
            }
            TextField{
                id: filterInput
                anchors.rightMargin: 5
                anchors.leftMargin: 5
                anchors.left: parent.left
                anchors.right: parent.right
                placeholderText: "filter ..."
                onTextChanged:{
                    if (text.length > 0){
                        filteredMaterials.clear()
                        for (var i =0; i < localMaterialsModel.count; ++i){
                            if (localMaterialsModel.get(i).name.toLowerCase().indexOf(text) !== -1)
                                filteredMaterials.append(localMaterialsModel.get(i))
                        }
                    } else {
                        filteredMaterials.clear()
                        for (var j =0; j < localMaterialsModel.count; ++j){
                            filteredMaterials.append(localMaterialsModel.get(j))
                        }
                    }
                }
            }
            Rectangle {
                color: myPalette.alternateBase
                anchors.rightMargin: 5
                anchors.leftMargin: 5
                anchors.topMargin: 5
                anchors.top: filterInput.bottom
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                Layout.fillHeight: true

                ListView {
                    id: listView
                    snapMode: ListView.SnapToItem
                    clip: true
                    model: ListModel{ id: filteredMaterials }
                    delegate: Text {
                        text: name
                        color: ListView.isCurrentItem ? myPalette.highlightedText : myPalette.text

                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                listView.currentIndex = index
                            }
                        }
                    }
                    spacing: 2
                    anchors.margins: 5
                    anchors.fill: parent
                    highlight: Rectangle {
                        color: myPalette.highlight
                        width: listView.width
                        height: listView.currentItem.height
                        y: listView.currentItem.y
                        x: listView.currentItem.x
                    }
                    highlightFollowsCurrentItem: false
                    focus: true
                    Component.onCompleted: {
                        for (var j =0; j < localMaterialsModel.count; ++j){
                            filteredMaterials.append(localMaterialsModel.get(j))
                        }
                    }
                }
            }
        }

        SplitView{
            id: centerItem
            anchors.rightMargin: 0
            anchors.right: rectangle1.left
            anchors.bottomMargin: 0
            anchors.leftMargin: 0
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.left: functions.right
            Layout.fillWidth: true
            Layout.minimumWidth: 200
            orientation: Qt.Vertical

            ColumnLayout{
                id: variableSection
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0
                Layout.minimumHeight: 100
                Layout.maximumHeight: 300

                Label{
                    id: variableLabel
                    text: "Variables"
                    anchors.topMargin: 5
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.top: parent.top
                }

                Rectangle {
                    color: myPalette.alternateBase
                    anchors.top: variableLabel.bottom
                    anchors.topMargin: 5
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5
                    anchors.rightMargin: 5
                    anchors.leftMargin: 5
                    anchors.right: parent.right
                    anchors.left: parent.left

                    ListView {
                        id: variableView
                        anchors.fill: parent
                        snapMode: ListView.SnapToItem
                        clip: true
                        model: VariablesModel{}
                        delegate: Text {
                            text: name
                            color: ListView.isCurrentItem ? myPalette.highlightedText : myPalette.text

                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    variableView.currentIndex = index
                                }
                            }
                        }
                        spacing: 2
                        anchors.margins: 5
                        highlight: Rectangle {
                            color: myPalette.highlight
                            width: variableView.width
                            height: variableView.currentItem.height
                            y: variableView.currentItem.y
                            x: variableView.currentItem.x
                        }
                        highlightFollowsCurrentItem: false
                        focus: true
                    }
                }
            }

            ColumnLayout{
                id: columnLayout1
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.bottom: parent.bottom
                Layout.minimumHeight: 300
                Layout.fillHeight: true

                Label{
                    id: codeLabel
                    text: "Code tree"
                    anchors.topMargin: 5
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.top: parent.top
                }
                Rectangle{
                    color: myPalette.alternateBase
                    anchors.top: codeLabel.bottom
                    anchors.topMargin: 5
                    anchors.bottomMargin: 5
                    anchors.rightMargin: 5
                    anchors.leftMargin: 5
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    Layout.fillHeight: true

                    ListView{
                        id: codeTreeView
                        anchors.fill: parent
                        model: CodeTreeModel{}
                        delegate: Text {
                            text: name
                            color: ListView.isCurrentItem ? myPalette.highlightedText : myPalette.text

                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    codeTreeView.currentIndex = index
                                }
                            }
                        }
                        spacing: 2
                        anchors.margins: 5
                        highlight: Rectangle {
                            color: myPalette.highlight
                            width: codeTreeView.width
                            height: codeTreeView.currentItem.height
                            y: codeTreeView.currentItem.y
                            x: codeTreeView.currentItem.x
                        }
                        highlightFollowsCurrentItem: false
                        focus: true
                    }
                }
            }
        }

        Rectangle{
            id: rectangle1
            color: "#00000000"
            anchors.rightMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.right: parent.right
            Layout.minimumWidth: 200
            Layout.maximumWidth: 400
        }

    }

    LocaleMaterialsModel{
        id: localMaterialsModel
    }
}
