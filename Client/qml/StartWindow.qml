import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3

Rectangle {
    property alias humanButton: humanButton
    property alias aiButton: aiButton
    property alias whiteButton: whiteButton
    property alias blackButton: blackButton
    property alias minBox: minBox
    property alias secBox: secBox
    property alias startButton: startButton
    property alias cancelButton: cancelButton
    
    id: main
    width: 380
    height: 180
    color: Universal.theme === Universal.Dark ? "#f0222222" : "#f0dddddd"
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    RowLayout {
        y: 56
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.verticalCenterOffset: -14
        anchors.verticalCenter: parent.verticalCenter
        spacing: 12
        ColumnLayout {
            spacing: 6
            RadioButton {
                id: humanButton
                text: qsTr("Human")
                checked: true
                checkable: !onlineWindow.visible
                down: onlineWindow.visible
            }
            RadioButton {
                id: aiButton
                text: qsTr("AI")
                checkable: !onlineWindow.visible
                down: onlineWindow.visible
            }
        } 
        ColumnLayout {
            spacing: 6
            RadioButton {
                id: whiteButton
                text: qsTr("White")
                checked: true
                checkable: aiButton.checked || onlineWindow.visible
                down: humanButton.checked && !onlineWindow.visible
            }
            RadioButton {
                id: blackButton
                text: qsTr("Black")
                checkable: aiButton.checked || onlineWindow.visible
                down: humanButton.checked && !onlineWindow.visible
            }
        }
        ColumnLayout {
            spacing: 6
            SpinBox {
                id: minBox
                editable: true
                to: 59
                value: 5
            }
            SpinBox {
                id: secBox
                editable: true
                to: 59
            }
        }
        ColumnLayout {
            Layout.preferredHeight: 70
            Layout.preferredWidth: 56
            spacing: 6
            Label {
                id: minLabel
                text: qsTr("Minutes")
            }
            Label {
                id: secLabel
                text: qsTr("Seconds")
            }
        }
    }
    
    RowLayout {
        spacing: 1
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        MyButton {
            id: startButton
            width: 90
            text: onlineWindow.visible ? qsTr("Create") : qsTr("Start")
            textColor: "#11ee11"
            Layout.preferredWidth: main.width / 2
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        MyButton {
            id: cancelButton
            width: 90
            text: qsTr("Cancel")
            textColor: "#ee1111"
            Layout.preferredWidth: main.width / 2
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}