import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3

Slider
{
    property string color1
    property string color2

    id: colorSlider
    from: 16
    stepSize: 1
    to: 255
    value: 255
    onValueChanged: window.Universal.accent = '#' +
                    rSlider.value.toString(16) +
                    gSlider.value.toString(16) +
                    bSlider.value.toString(16)
    background: Rectangle
    {
        x: rSlider.leftPadding
        y: rSlider.topPadding + rSlider.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 4
        width: rSlider.availableWidth
        height: 2
        radius: 2
        gradient: Gradient
        {
            orientation: Gradient.Horizontal

            GradientStop { position: 0.0; color: color1 }
            GradientStop { position: 1.0; color: color2 }
        }
    }
}
