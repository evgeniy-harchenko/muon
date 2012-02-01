import QtQuick 1.1
import org.kde.plasma.components 0.1

Rectangle {
    height: 400
    width: 500
    color: "lightgrey"
    property Component categoryComp: Qt.createComponent("qrc:/qml/CategoryView.qml")
    property Component applicationListComp: Qt.createComponent("qrc:/qml/ApplicationsList.qml")
    property Component applicationComp: Qt.createComponent("qrc:/qml/ApplicationView.qml")
    
    function openApplicationList(cat) {
        try {
            var obj = applicationListComp.createObject(pageStack, { category: cat })
            console.log("holaaa "+obj)
            pageStack.push(obj);
            breadcrumbs.pushItem("user-home", "cosa")
        } catch (e) {
            console.log("error: "+e)
            console.log("comp error: "+applicationListComp.errorString())
        }
    }
    
    function openCategory(cat) {
        try {
            var obj = categoryComp.createObject(pageStack, { category: cat })
            pageStack.push(obj);
            breadcrumbs.pushItem("go-home", "hola")
        } catch (e) {
            console.log("error: "+e)
        }
    }
    
    function openApplication(app) {
        try {
            var obj = applicationComp.createObject(pageStack, { application: app })
            console.log("holaaa "+obj)
            pageStack.push(obj);
            breadcrumbs.pushItem(app.icon, app.name)
        } catch (e) {
            console.log("error: "+e)
            console.log("comp error: "+applicationComp.errorString())
        }
    }
    
    ToolBar {
        id:toolbar
        z: 10
        height: 40
        width: parent.width
        anchors.top: parent.top
        clip: true
        
        Breadcrumbs {
            id: breadcrumbs
            anchors.fill: parent
            onClicked: {
                var pos = idx;
                while(pos--) { pageStack.pop(); breadcrumbs.popItem() }
            }
        }
    }
    
    PageStack
    {
        id: pageStack
        width: parent.width
        anchors.bottom: parent.bottom
        anchors.top: toolbar.bottom
        initialPage: CategoryView {}
        
        toolBar: toolbar
    }
}