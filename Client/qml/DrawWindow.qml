import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3

Rectangle
{
    property alias drawYesButton: drawYesButton
    property alias drawNoButton: drawNoButton
    
    id: main
    width: 380
    height: 180
    color: Universal.theme === Universal.Dark ? "#f0222222" : "#f0dddddd"
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    
    Label
    {
       text: "Opponent offers you a draw"
       font.pointSize: 12
       anchors.horizontalCenter: parent.horizontalCenter
       topPadding: main.height/2 - 28
    }
    
    RowLayout
    {
        spacing: 1
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        MyButton
        {
            id: drawYesButton
            width: 90
            text: qsTr("Accept")
            textColor: "#11ee11"
            Layout.preferredWidth: main.width / 2
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        MyButton
        {
            id: drawNoButton
            width: 90
            text: qsTr("Deny")
            textColor: "#ee1111"
            Layout.preferredWidth: main.width / 2
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
