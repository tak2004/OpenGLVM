import QtQuick 2.2
import QtQuick.Controls 1.3

ApplicationWindow {
    id: applicationWindow1
    width: 800
    height: 600
    visible: true
    minimumWidth: 800
    minimumHeight: 600
    title: "OGLIF Builder"

    menuBar: MenuBar{
        Menu{
            title: "File"
            MenuItem{text:"New"}
            MenuItem{text:"Open"}
            MenuItem{text:"Save"}
            MenuSeparator{}
            MenuItem{text:"Exit"}
        }
        Menu{
            title: "Tools"
            MenuItem{text:"Options"}
            MenuItem{text:"Customize"}
        }
    }

    TabView{
        anchors.fill: parent
        Tab{
            title: "Builder"
            RemoteBrowser{

            }
        }
    }
}

