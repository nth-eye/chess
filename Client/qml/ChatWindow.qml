import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3

ColumnLayout {
    property alias finishButton: finishButton
    
    property alias stopButtons: stopButtons
    property alias claimDrawButton: claimDrawButton
    property alias offerDrawButton: offerDrawButton
    property alias resignButton: resignButton
    property alias drawTimer: drawTimer
    
    property alias ipField: ipField
    property alias disconnectButton: disconnectButton
    property alias dialogueText: dialogueText
    
    property alias messageField: messageField
    property alias sendButton: sendButton

    width: 288
    height: boardWindow.height - 18
    anchors.right: boardWindow.left
    anchors.top: boardWindow.top
    anchors.rightMargin: 39
    anchors.topMargin: 9
    spacing: 12
    MyButton {
        id: finishButton
        text: qsTr("Finish")
        textColor: "#ee1111"
        Layout.fillWidth: true
        Layout.fillHeight: false
        enabled: true
        visible: false
    }
    RowLayout {
        id: stopButtons
        Layout.fillWidth: true
        visible: !finishButton.visible
        spacing: 12
        Button {
            id: claimDrawButton
            text: qsTr("Claim draw")
            Layout.fillWidth: true
            enabled: false;
        }
        Button {
            id: offerDrawButton
            text: qsTr("Offer draw")
            Layout.fillWidth: true
            enabled: false;
        }
        Button {
            id: resignButton
            text: qsTr("Resign")
            Layout.fillWidth: true
        }
        Timer {
            id: drawTimer
            interval: 30000
            repeat: false
        }
    }
    ColumnLayout {
        Layout.fillWidth: true
        spacing: -2
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: false
            spacing: 0
            TextField {
                id: ipField
                Layout.fillHeight: true
                Layout.fillWidth: true
                placeholderText: "IP address..."
                enabled: false
            }
            MyButton {
                id: disconnectButton
                text: "Disconnect"
                textColor: "#ee1111"
                Layout.fillHeight: true
                Layout.maximumWidth: 100
                Layout.minimumWidth: 100
                Layout.preferredWidth: 100
                enabled: false
                Rectangle {
                    color: "#888888"
                    height: parent.height
                    width: 2
                }
            }
        }
        Rectangle {
            color: "#888888"
            Layout.fillWidth: true
            height: 2
            z: 1
        }
        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            TextArea.flickable: TextArea {
                id: dialogueText
                color: Universal.accent
                readOnly: true
                selectByMouse: true
                placeholderText : qsTr("Dialogue text...")
                placeholderTextColor: "#888888"
                textFormat: TextEdit.RichText
                wrapMode: TextEdit.Wrap 
                Layout.fillWidth: true
                Layout.fillHeight: true
                background: Rectangle {
                    color: settingsPage.themeSwitch.checked ? "#cccccc" : "#333333"
                }
            }
            ScrollBar.vertical: ScrollBar {
                width: 2
            }
        }
    }
    RowLayout {
        spacing: 6
        Layout.fillWidth: true
        Layout.fillHeight: false
        Layout.preferredHeight: 32
        TextField {
            id: messageField
            placeholderText: qsTr("Message...")
            Layout.fillWidth: true
            Layout.fillHeight: true
            selectByMouse: true
            enabled: disconnectButton.enabled
        }
        Button {
            id: sendButton
            text: qsTr("Send")
            Layout.maximumWidth: 60
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            enabled: messageField.length > 0 && disconnectButton.enabled
        }
    }
}
