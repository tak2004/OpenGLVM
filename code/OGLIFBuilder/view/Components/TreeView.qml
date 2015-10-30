import QtQuick 2.1
import QtQuick.Controls 1.0
 
ScrollView {
    id: view    
 
    property var model
    property bool restructure: false
    property int rowHeight: 19
    property int controlHeight: 19
    property int columnIndent: 22
    property var currentNode
    property var currentItem
    property ListModel mapping: ListModel{}
 
    property SystemPalette myPalette : SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }
    
    function toJSObj(data)
    {
        var _item = {};
        for ( var item_index in data )
        {
            if ( item_index === "index" )
                continue; // do nothing
            if ( item_index === "model" )
                continue; // do nothing
            if ( item_index === "objectName" )
                continue; // do nothing
            if ( typeof(data[item_index]) == 'undefined' )
                continue; // do nothing
            if ( typeof(data[item_index]) == 'function' )
                continue; // do nothing
            
            if ( typeof(data[item_index]) == 'object' && data[item_index].count !== undefined )
            {
                _item[item_index] = [];
                for ( var i = 0; i < data[item_index].count; i++ )
                {
                    _item[item_index].push(toJSObj(data[item_index].get(i)));
                }
            }
            else
            {
                _item[item_index] = data[item_index];
            }
        }
        return _item;
    }
    
    
    property variant delegate: QtObject
    {
        property Component label: Label {
            wrapMode: Text.WordWrap
            text: model.text ? model.text : 0
            color: currentNode === model ? myPalette.highlightedText : myPalette.text
        }
    }
    
    /// signal is global.. The timer can not be added to scrollview...
    signal itemDelete(variant item_model, int item_index)

    onItemDelete:
    {
        // Set edit mode... (this clears the loader to avoid crash)
        // Mind you the editmode and delay cause flickering
        content.modelEdit = true;
        
        // Setup delete vars and start the timer (also to avoid crash)
        delayed_delete.the_index = item_index
        delayed_delete.the_model = item_model
        delayed_delete.start();
    }
    
    
    frameVisible: true
    implicitWidth: 200
    implicitHeight: 160
 
    contentItem: Loader {
        id: content
        property bool modelEdit: false
        onLoaded: item.isRoot = true
        
        Timer
        {
            id: delayed_delete
            interval: 1
            property variant the_model
            property int the_index
            onTriggered:
            {
                the_model.remove(the_index);
                // Disable the model edit
                content.modelEdit = false;
            }
        }
 
        
        sourceComponent: modelEdit ? undefined : treeBranch
        property var elements: model
        
        Column {
            anchors.fill: parent
            Repeater {
                model: 1 + Math.max(view.contentItem.height, view.height) / rowHeight
                Rectangle {
                    objectName: "Faen"
                    color: index % 2 ? myPalette.alternateBase : myPalette.base
                    width: view.width ; height: rowHeight
                    
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: parent.color = myPalette.highlight
                        onExited: parent.color = index % 2 ? myPalette.alternateBase : myPalette.base
                    }
                }
            }
        }
        Component {
            id: treeBranch
            Item {
                id: root
                property bool isRoot: false
                implicitHeight: column.implicitHeight
                implicitWidth: column.implicitWidth
                Column {
                    id: column
                    x: 2
                    Item { height: isRoot ? 0 : rowHeight; width: 1}
                    Repeater {
                        model: elements
                        Item {
                            id: filler
                            
                            Image
                            {
                                anchors.left: parent.left
                                anchors.top: rowfill.top
                                height: rowfill.height/3
                                fillMode: Image.PreserveAspectFit
                                id: before_indicator
                                source: ""
                                smooth: true
                            }
                            Image
                            {
                                anchors.left: parent.left
                                anchors.verticalCenter: rowfill.verticalCenter
                                height: rowfill.height/3
                                fillMode: Image.PreserveAspectFit
                                id: on_indicator
                                source: ""
                                smooth: true
                            }
                            Image
                            {
                                anchors.left: parent.left
                                anchors.bottom: rowfill.bottom
                                height: rowfill.height/3
                                fillMode: Image.PreserveAspectFit
                                id: after_indicator
                                source: ""
                                smooth: true
                            }
                            
                            DropArea
                            {
                                anchors.left: rowfill.left
                                anchors.right: rowfill.right
                                anchors.top: rowfill.top
                                height: rowfill.height/3
                                keys: [ "treeItem" ]
                                
                                onDropped:
                                {
                                    before_indicator.source = ""
                                    // Assume node id is unique -> we don't allow dropping on our self..
                                    if ( drop.source.dragItem.node_id == elements.get(model.index).node_id )
                                        return;
                                    
                                    // Build item
                                    var _item = toJSObj(drop.source.dragItem);
                                    
                                    /// trigger remove the old element
                                    itemDelete(drop.source.dragModel, drop.source.dragItem.index);
                                    elements.insert(model.index, _item);
                                }
                                onEntered:
                                {
                                    before_indicator.source = 'images/indicator.png'
                                }
                                onExited:
                                {
                                    before_indicator.source = ""
                                }
                            }
                            DropArea
                            {
                                anchors.left: rowfill.left
                                anchors.right: rowfill.right
                                anchors.verticalCenter: rowfill.verticalCenter
                                height: rowfill.height/3
                                keys: [ "treeItem" ]
                               
                                onDropped:
                                {
                                    on_indicator.source = ""
                                    // Assume node id is unique -> we don't allow dropping on our self..
                                    if ( drop.source.dragItem.node_id == elements.get(model.index).node_id )
                                        return;
                                    
                                    // Build item
                                    var _item = toJSObj(drop.source.dragItem);
                                    
                                    /// trigger remove the old element
                                    itemDelete(drop.source.dragModel, drop.source.dragItem.index);
                                    /// apend to childNodes
                                    elements.get(model.index).childNodes.append(_item);
                                }
                                onEntered:
                                {
                                    on_indicator.source = 'images/indicator.png'
                                }
                                onExited:
                                {
                                    on_indicator.source = ""
                                }
                            }
                            DropArea
                            {
                                anchors.left: rowfill.left
                                anchors.right: rowfill.right
                                anchors.bottom: rowfill.bottom
                                height: rowfill.height/3
                                keys: [ "treeItem" ]
                                
                                onDropped:
                                {
                                    after_indicator.source = ""
                                    // Assume node id is unique -> we don't allow dropping on our self..
                                    if ( drop.source.dragItem.node_id == elements.get(model.index).node_id )
                                        return;
                                    
                                    // Build item
                                    var _item = toJSObj(drop.source.dragItem);
                                    
                                    /// trigger remove the old element
                                    itemDelete(drop.source.dragModel, drop.source.dragItem.index);
                                    elements.insert(model.index+1, _item);
                                }
                                onEntered:
                                {
                                    after_indicator.source = 'images/indicator.png'
                                }
                                onExited:
                                {
                                    after_indicator.source = ""
                                }
                            }
                            
                            width: Math.max(loader.width + columnIndent, row.width)
                            height: Math.max(row.height, loader.height)
                            property var _model: model
                            Rectangle {
                                id: rowfill
                                x: view.mapToItem(rowfill, 0, 0).x
                                width: view.width
                                height: rowHeight
                                visible: currentNode === model
                                color: "#37f"
                            }
                            MouseArea
                            {
                                property bool startdrag : false
                                property int mousex: 0
                                property int mousey: 0

                                id: dragArea
                                visible: view.restructure
                                anchors.fill: rowfill
                                drag.axis: Drag.YAxis
                                onPressed:
                                {
                                    startdrag = true
                                    mousex = mouse.x
                                    mousey = mouse.y
                                }
                                onPositionChanged: {
                                    if (startdrag === true &&
                                       (Math.abs(mouse.x - mousex) > drag.threshold ||
                                        Math.abs(mouse.y - mousey) > drag.threshold))
                                    {
                                        row.z = 2
                                        row._dragging = true
                                        dragArea.drag.target = row
                                        loader.expanded = false
                                        startdrag = false
                                    }
                                }
                                onReleased:
                                {
                                    startdrag = false
                                    if ( row._dragging )
                                    {
                                        row.Drag.drop()
                                        dragArea.drag.target = null
                                        row._dragging = false;
                                    }
                                }
                            }
                            Rectangle
                            {
                                anchors.fill: row
                                color: row._dragging ? Qt.rgba(0,0,0,0.3) : "transparent"
                            }
                            Row {
                                id: row
                                Drag.keys: [ "treeItem" ]
                                Drag.active: dragArea.drag.active
                                Drag.source: QtObject
                                    {
                                        property variant dragModel: elements
                                        property variant dragItem: model
                                    }
                                property bool _dragging: false
                                
                                z: _dragging ? 10 : 1
                                y: _dragging ? 10 : 0
                                
                                Item {
                                    width: controlHeight
                                    height: rowHeight
                                    opacity: !!model.childNodes.count > 0 ? 1 : 0
                                    Image {
                                        id: expander
                                        source: "images/expander.png"
                                        opacity: mouse.containsMouse ? 1 : 0.7
                                        anchors.centerIn: parent
                                        rotation: loader.expanded ? 90 : 0
                                        Behavior on rotation {NumberAnimation { duration: 120}}
                                    }
                                    MouseArea {
                                        id: mouse
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked:
                                        {
                                            loader.expanded = !loader.expanded
                                            elements.setProperty(model.index, "expanded", loader.expanded);
                                        }
                                    }
                                }
                                Loader {
                                    property int offset_left: loader.x
                                    property int offset_right: 2
                                    width: rowfill.width - offset_left - offset_right
                                    property var model: _model
                                    sourceComponent: delegate.label
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                            Loader {
                                id: loader
                                x: columnIndent
                                height: expanded ? implicitHeight : 0
                                property bool expanded: model.expanded ? model.expanded : false
                                property var elements: model.childNodes
                                sourceComponent: (expanded && !!model.childNodes) ? treeBranch : undefined
                            }
                        }
                    }
                }
            }
        }
    }
}
