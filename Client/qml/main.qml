import QtQuick 2.2
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3

ApplicationWindow {
    Universal.theme: settingsPage.themeSwitch.checked ? Universal.Light : Universal.Dark
    
    id: window
    visible: true
    width: 1240
    height: 720
    title: qsTr("Tabs")
    
    SwipeView {
        id: swipeView
        interactive: false
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        GamePage {
            id: gamePage
        }

        SettingsPageForm {
            id: settingsPage
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {text: qsTr("Game")}
        TabButton {text: qsTr("Settings")}
    }
}
