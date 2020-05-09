import QtQuick 2.0
import QtQuick.Controls.Universal 2.3

Rectangle {

    property alias clockTime: clockTime
    
    id: clock
    width: 110
    height: 40
    radius: 3
    Text {
        id: clockTime
        text: "00:00:0"
        font.pointSize: 14
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }
    states: [
        State {
            name: "RUNNING"
            PropertyChanges {
                target: clock
                color: Universal.theme === Universal.Dark ? "#ffffff" : "#000000"
            }
            PropertyChanges {
                target: clockTime
                color: Universal.theme === Universal.Dark ? "#000000" : "#ffffff"
            }
        },
        State {
            name: "STOPPED"
            PropertyChanges {
                target: clock
                color: Universal.theme === Universal.Dark ? "#000000" : "#ffffff"
            }
            PropertyChanges {
                target: clockTime
                color: Universal.theme === Universal.Dark ? "#ffffff" : "#000000"
            }
        }

    ]
}
