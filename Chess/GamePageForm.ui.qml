import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3

Page {
    property alias gameStatus: gameStatus
    property alias whiteClock: whiteClock
    property alias blackClock: blackClock
    
    property alias queenButton: queenButton
    property alias rookButton: rookButton
    property alias bishopButton: bishopButton
    property alias knightButton: knightButton
    
    property alias promotionWindow: promotionWindow
    property alias boardWindow: boardWindow
    property alias chatWindow: chatWindow
    property alias startWindow: startWindow
    property alias onlineWindow: onlineWindow
    property alias drawWindow: drawWindow
    
    property alias onlineButton: onlineButton
    property alias offlineButton: offlineButton
    
    id: gamePage
    width: 1240
    height: 720

    header: Label {
        text: "Chess"
        color: Universal.accent
        font.pixelSize: 36
        horizontalAlignment: Text.AlignHCenter
        padding: 10
    }
    
    ColumnLayout {
        id: mainButtons
        width: 240
        height: 120
        anchors.centerIn: parent
        visible: !boardWindow.visible
        enabled: !startWindow.visible && !onlineWindow.visible
        spacing: 20
        MyButton {
            id: onlineButton
            text: qsTr("Online")
            textColor: Universal.accent
            font.pointSize: 12
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        MyButton {
            id: offlineButton
            text: qsTr("Offline")
            textColor: Universal.accent
            font.pointSize: 12
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
    
    Rectangle {
        id: boardWindow
        width: 498
        height: 498
        color: "#00000000"
        border.width: 4
        border.color: Universal.accent
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
        Clock {
            id: whiteClock
            anchors.bottom: parent.top
            anchors.left: parent.left
            anchors.bottomMargin: 8
            visible: false
            state: "RUNNING"
        }
        Clock {
            id: blackClock
            anchors.bottom: parent.top
            anchors.right: parent.right
            anchors.bottomMargin: 8
            visible: false
            state: "STOPPED"
        }
        Label {
            id: gameStatus
            text: qsTr("Welcome!")
            font.pixelSize: 20
            anchors.top: parent.top
            anchors.topMargin: -44
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
        }
        Image {
            id: boardImg
            source: "/images/board.png"
            width: 480
            height: 480
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            objectName: "boardImg"
        }
        Rectangle {
            id: promotionWindow
            color: Universal.theme === Universal.Dark ? "#f0222222" : "#f0dddddd"
            width: 360
            height: 120
            anchors.centerIn: parent
            visible: false
            RowLayout {
                anchors.centerIn: parent
                height: 60
                width: 300
                spacing: 20
                Button {
                    id: queenButton
                    icon.source: "/images/white_queen.png"
                    icon.width: parent.width
                    icon.height: parent.height
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                Button {
                    id: rookButton
                    icon.source: "/images/white_rook.png"
                    icon.width: parent.width
                    icon.height: parent.height
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                Button {
                    id: bishopButton
                    icon.source: "/images/white_bishop.png"
                    icon.width: parent.width
                    icon.height: parent.height
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                Button {
                    id: knightButton
                    icon.source: "/images/white_knight.png"
                    icon.width: parent.width
                    icon.height: parent.height
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }
    }
    
    ChatWindow {
        id: chatWindow
        visible: boardWindow.visible
    }
    
    OnlineWindow {
        id: onlineWindow
        visible: false
    }
    
    StartWindow {
        id: startWindow
        visible: false
    }
    
    DrawWindow {
        id: drawWindow
        visible: false
    }
}
