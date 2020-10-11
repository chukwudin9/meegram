import QtQuick 1.1
import com.nokia.meego 1.1

PageStackWindow {
    id: appWindow

    initialPage: mainPage

    MainView {
        id: mainPage
    }

    ToolBarLayout {
        id: commonTools
        visible: true
        ToolIcon {
            platformIconId: "toolbar-view-menu"
            anchors.right: (parent === undefined) ? undefined : parent.right
            onClicked: (myMenu.status === DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }
    }

    Menu {
        id: myMenu
        MenuLayout {
            MenuItem {
                text: qsTr("Quit")
                onClicked: Qt.quit()
            }
        }
    }
}
