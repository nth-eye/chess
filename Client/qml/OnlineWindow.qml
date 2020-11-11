import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import chess.game 1.0
import chess.client 1.0

ColumnLayout {
    property alias serverField: serverField
    property alias serverButton: serverButton
    property alias refreshButton: refreshButton
    property alias busyIndicator: busyIndicator
    property alias onlineLabel: onlineLabel
    property alias gameList: gameList
    property alias acceptButton: acceptButton
    property alias createButton: createButton
    property alias closeButton: closeButton
    
    id: main
    width: 660
    height: 360
    spacing: 0
    visible: true
    anchors.centerIn: parent 
    RowLayout {
        Layout.preferredHeight: 30
        Layout.fillHeight: false
        Layout.fillWidth: true
        spacing: 8
        TextField {
            id: serverField
            text: qsTr("127.0.0.1")
            placeholderText: qsTr("Server IP address")
            font.pixelSize: 14
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: main.width / 4
            selectByMouse: true
            enabled: !serverButton.down
        }
        MyButton {
            id: serverButton
            text: down ? qsTr("Disconnect from server") : qsTr("Connect to server")
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: main.width / 2
            textColor: down ? "#ee1111" : Universal.accent
            down: false
        }
        MyButton {
            id: refreshButton
            text: qsTr("Refresh")
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: main.width / 4
            enabled: onlineWindow.state === "CONNECTED"
        }
    }
    Rectangle {
        color: Universal.theme === Universal.Dark ? "#f0666666" : "#f0999999"
        Layout.fillHeight: false
        Layout.fillWidth: true
        Layout.preferredHeight: 24
        RowLayout {
            anchors.rightMargin: 8
            anchors.leftMargin: 8
            anchors.fill: parent
            spacing: 20    
            Label {
                id: ipColumn
                text: "IP address"
                Layout.fillWidth: true
            }
            Label {
                id: portColumn
                text: "Port"
                Layout.fillWidth: true
            }
            Label {
                id: sideColumn
                text: "Your side"
                Layout.fillWidth: true
            }
            Label {
                id: timeColumn
                text: "Time limit"
                Layout.fillWidth: true
            }
        }
    }
    Rectangle {
        id: rectangle
        color: Universal.theme === Universal.Dark ? "#f0222222" : "#f0dddddd"
        Layout.fillHeight: true
        Layout.fillWidth: true
        Label {
            id: onlineLabel
            text: "Disconnected from server"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent
            visible: true
        }
        BusyIndicator {
            id: busyIndicator
            width: parent.width / 8
            height: parent.width / 8
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            running: !serverButton.enabled && !startWindow.visible
        }
        ListView {
            id: gameList
            anchors.bottomMargin: 2
            anchors.topMargin: 2
            anchors.rightMargin: 4
            anchors.leftMargin: 4
            anchors.fill: parent
            visible: !onlineLabel.visible
            clip: true
            highlight: Rectangle {
                color: Universal.accent
                radius: 2
                opacity: 0.3
            }
            ScrollIndicator.vertical: ScrollIndicator {}
            snapMode: ListView.SnapToItem
            delegate: Item {
                width: parent.width
                height: gameList.height / 10
                Row {
                    x: 4
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 20
                    Label {
                        width: ipColumn.width
                        color: Universal.foreground
                        text: IP
                    }
                    Label {
                        width: portColumn.width
                        color: Universal.foreground
                        text: port
                    }
                    Rectangle {
                        width: sideColumn.width
                        height: parent.height
                        color: colorCode
                    }
                    Label {
                        width: timeColumn.width
                        color: Universal.foreground
                        text: time
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: gameList.currentIndex = index
                }
            }
            model: Client.model
        }    
    }
    RowLayout {
        Layout.fillHeight: false
        Layout.fillWidth: true
        Layout.preferredHeight: 30
        spacing: 8
        MyButton {
            id: acceptButton
            text: qsTr("Accept game")
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: main.width / 4
            enabled: gameList.visible && gameList.currentItem
        }
        MyButton {
            id: createButton
            text: down ? qsTr("Cancel game") : qsTr("Create game")
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: main.width / 2
            textColor: down ? "#ee1111" : Universal.foreground
            enabled: false
        }
        MyButton {
            id: closeButton
            text: qsTr("Close")
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: main.width / 4
            textColor: "#ee1111"
            enabled: !createButton.down
        }
    }

    states: [
        State {
            name: "DISCONNECTED"
            PropertyChanges {
                target: serverButton; 
                enabled: true
                down: false
            }
            PropertyChanges {
                target: onlineLabel; 
                visible: true
                text: "Disconnected from server"
            }
            PropertyChanges {
                target: createButton; 
                enabled: false
                down: false
            }
        },
        State {
            name: "CONNECTED"
            PropertyChanges {
                target: serverButton; 
                enabled: true
                down: true
            }
            PropertyChanges {
                target: onlineLabel; 
                visible: !(gameList.count > 0)
                text: "No games available"
            }
            PropertyChanges {
                target: createButton; 
                enabled: true
                down: false
            }
        },
        State {
            name: "CONNECTING"
            PropertyChanges {
                target: serverButton; 
                enabled: false
                down: true
            }
            PropertyChanges {
                target: onlineLabel; 
                visible: true
                text: "Connecting to server..."
            }
            PropertyChanges {
                target: createButton;
                enabled: false
                down: false
            }
        },
        State {
            name: "WAITING"
            PropertyChanges {
                target: serverButton; 
                enabled: false
                down: true
            }
            PropertyChanges {
                target: onlineLabel; 
                visible: true
                text: "Waiting for opponent..."
            }
            PropertyChanges {
                target: createButton; 
                enabled: true
                down: true
            }
        }
    ]
    
}
