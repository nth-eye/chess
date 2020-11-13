import QtQuick 2.2
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3

Page
{
    property alias themeSwitch: themeSwitch

    width: 1240
    height: 720
    header: Label
    {
        id: settingsHeader
        text: qsTr("Settings")
        padding: 10
        font.pixelSize: 30
        horizontalAlignment: Text.AlignHCenter
    }
    GridLayout
    {
        anchors.left: parent.left
        anchors.leftMargin: 69
        anchors.top: parent.top
        anchors.topMargin: 69
        columns: 2
        rows: 4

        Label { text: qsTr("Theme") }
        Switch
        {
            id: themeSwitch
            checked: false
        }

        Label { text: qsTr("Color") }
        ColumnLayout
        {
            spacing: 0

            ColorSlider { id: rSlider; color1: "#100000"; color2: "#ff0000" }
            ColorSlider { id: gSlider; color1: "#001000"; color2: "#00ff00" }
            ColorSlider { id: bSlider; color1: "#000010"; color2: "#0000ff" }
        }

        Label { text: qsTr("Max AI depth") }
        Slider
        {
            id: depthSlider
            snapMode: Slider.SnapOnRelease
            stepSize: 1
            to: 10
            from: 1
            value: 6
            objectName: "depthSlider"

            Label
            {
                color: depthSlider.pressed ? "#ffffff" : "#aaaaaa"
                text: depthSlider.value
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.right
            }
        }
    }
}
