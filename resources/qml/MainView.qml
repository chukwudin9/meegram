import QtQuick 1.1
import com.nokia.meego 1.1

Page {
    tools: commonTools

    Label {
        id: label
        anchors.centerIn: parent
        text: qsTr("Still testing account")
    }

    Button{
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: label.bottom
            topMargin: 10
        }
        text: qsTr("Get chat")
        onClicked: ChatStore.getChats(75, 95985, 0, 233)
    }
}

