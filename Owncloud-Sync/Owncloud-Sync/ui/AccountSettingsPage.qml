import QtQuick 2.4
import Ubuntu.Components 1.3
import "../components"

Page {
    id: accountSettingsPage

    // Page header trailing action
    property Action trailingAction

    // Signal fired when connect button is clicked
    signal connectClicked(string username, string password, int timer, string serverURL, bool mobileData, string lastSync)

    property var accountSettings
    
    header: PageHeader {
        id: header
        title: i18n.tr("Owncloud Settings")
        flickable: accountSettingsFlickable
        trailingActionBar.actions: accountSettingsPage.trailingAction
    }
    
    Flickable {
        id: accountSettingsFlickable
        
        anchors.fill: parent
        contentHeight: accountSettingsColumn.height
        
        Column {
            id: accountSettingsColumn
            
            spacing: units.gu(1.5)
            anchors { top: parent.top; left: parent.left; right: parent.right; margins: units.gu(2) }
            
            FormTextField {
                id: username
                title.text: i18n.tr("Username")
                textfield.placeholderText: i18n.tr("Username")
                textfield.text: accountSettings.username
            }
            
            FormTextField {
                id: password
                title.text: i18n.tr("Password")
                textfield.text: accountSettings.password
                textfield.placeholderText: i18n.tr("Password")
                textfield.echoMode: TextInput.Password
            }
            
            Label {
                id: frequencyLabel
                text: i18n.tr("Sync Frequency")
            }
            
            OptionSelector {
                id: frequency
                width: parent.width
                property int timer: accountSettings.timer
                model: [15, 30, 45, 60, 90, 120]
                
                delegate: OptionSelectorDelegate{text: frequency.model[index] + " " + i18n.tr("minutes")}
                
                onSelectedIndexChanged: {
                    timer = model[selectedIndex] * 60;
                    print("Index Changed: " + model[selectedIndex]);
                }
            }
            
            FormTextField {
                id: serverURL
                title.text: i18n.tr("Server URL")
                textfield.placeholderText: "https://myurl.com/owncloud"
                textfield.hasClearButton: true
                textfield.text: accountSettings.serverURL
            }
            
            Item {
                width: parent.width
                height: mobileDataLabel.implicitHeight + units.gu(1)
                
                Label{
                    id: mobileDataLabel
                    text: i18n.tr("Sync on Mobile Data")
                    anchors { left: parent.left; right: mobileData.left; verticalCenter: parent.verticalCenter }
                }
                
                Switch{
                    id: mobileData
                    checked: accountSettings.mobileData
                    anchors { right: parent.right; verticalCenter: parent.verticalCenter }
                }
            }
            
            Button {
                width: parent.width
                text: i18n.tr("Connect")
                color: UbuntuColors.green
                onClicked: connectClicked(username.textfield.text, password.textfield.text, frequency.timer, serverURL.textfield.text, mobileData.checked, lastSyncLabel.lastSyncTime)
            }
            
            Item{
                anchors.horizontalCenter: parent.horizontalCenter
                width: units.gu(15)
                height: width * 1.25
                
                Rectangle{
                    id: connectionIndicator
                    color: apl.testingConnection ? UbuntuColors.orange : (apl.connected ? UbuntuColors.green : UbuntuColors.red)
                    anchors{ verticalCenter:parent.verticalCenter; horizontalCenter: parent.horizontalCenter}
                    width: parent.width * 0.85
                    height: width
                    radius: width / 2
                }
                
                Icon {
                    anchors{ horizontalCenter: connectionIndicator.horizontalCenter; verticalCenter: connectionIndicator.verticalCenter}
                    width: units.gu(4)
                    height: width
                    color: UbuntuColors.porcelain
                    name: apl.connected ? "tick" : "close"
                }
                
                Label {
                    text: apl.connected ? i18n.tr("Connected") : i18n.tr("Not Connected")
                    anchors{ horizontalCenter: parent.horizontalCenter; top: connectionIndicator.bottom; topMargin: units.gu(2)}
                }
                
                SequentialAnimation {
                    running: apl.testingConnection
                    loops: Animation.Infinite
                    OpacityAnimator {
                        target: connectionIndicator;
                        from: 1;
                        to: 0.2;
                        duration: 1000
                    }
                    
                    OpacityAnimator {
                        target: connectionIndicator;
                        from: 0.2;
                        to: 1;
                        duration: 1000
                    }
                }
            }
            
            Label{
                id: lastSyncLabel
                visible: lastSyncTime
                property string lastSyncTime: accountSettings.lastSync
                text: i18n.tr("Last Sync: ") + lastSyncTime
            }
        }
    }
}
