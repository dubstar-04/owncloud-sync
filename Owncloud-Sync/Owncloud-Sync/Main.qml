/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.Pickers 1.0
import "ui"
import "components"

// C++ Plugin
import OwncloudSync 1.0


import Qt.labs.settings 1.0
import Qt.labs.folderlistmodel 2.1
import QtQuick.LocalStorage 2.0
//import QtQuick.XmlListModel 2.0

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "owncloud-sync"

    anchorToKeyboard: true

    ServiceController {
        id: serviceController
        serviceName: "OwncloudSyncd"
        Component.onCompleted: {

            //Create the upstart files
            if (!serviceController.serviceFileInstalled) {
                print("Service file not installed. Installing now.")
                serviceController.installServiceFile();
            }

            //stop the sync service
            //The service should be stopped here incase the user is trying to change the mobile data
            //The other option is restarting the daemon after every change?
            if (serviceController.serviceRunning) {
                print("Service not running. Starting now.")
                serviceController.stopService();
            }

        }

        Component.onDestruction: {

            //start the sync service
            if (!serviceController.serviceRunning) {
                print("Service not running. Starting now.")
                serviceController.startService();
            }

        }
    }

    function testConnection(username, password, timer, serverURL, mobileData, lastSync){
        apl.testingConnection = true;

        var req = new XMLHttpRequest();
        var location = "https://" + username + ":" + password + "@" +
                serverURL + "/remote.php/webdav/"

        console.log("TestConnection() - URL: " + location)

        //tell the request to go ahead and get the json
        req.open("GET", location, true);
        req.send(null);

        //wait until the readyState is 4, which means the json is ready
        req.onreadystatechange = function()
        {
            if (req.readyState == 4)
            {
                if (req.status == 200){
                    //console.log(req.responseText);
                    apl.connected = true
                    apl.testingConnection = false;

                    // Only save the account credentials once they are verified to be correct
                    accountSettings.username = username
                    accountSettings.password = password
                    accountSettings.serverURL = serverURL
                    accountSettings.timer = timer
                    accountSettings.lastSync = lastSync

                    //turn the text in a javascript object while setting the ListView's model to it
                    //fileList.model = JSON.parse(req.responseText)
                }
                else{
                    apl.connected = false
                    apl.testingConnection = false;
                }
            }
        };


    }


    width: units.gu(60)
    height: units.gu(75)

    AdaptivePageLayout {
        id: apl
        property int windowWidth: width/units.gu(1)
        property int maxWidth: 61 //width in grid units
        property bool connected: false
        property bool testingConnection: true
        anchors.fill: parent
        primaryPageSource: ocSettings

        onWindowWidthChanged: {
            if( windowWidth > maxWidth){
                apl.addPageToNextColumn(apl.primaryPageSource, syncSettings)
            } else {
                apl.removePages(syncSettings)
            }

        }

        Component.onCompleted:
        {
            testConnection();
        }


        Settings {
            id: accountSettings
            property string timer
            property string password
            property string username
            property string serverURL
            property string lastSync
            property bool mobileData
        }

        layouts: PageColumnsLayout {
            when: width > units.gu(apl.maxWidth)

            // column #0
            PageColumn {
                minimumWidth: units.gu(10)
                maximumWidth: units.gu(apl.maxWidth)
                preferredWidth: units.gu(40)

            }
            // column #1
            PageColumn {
                fillWidth: true

            }
        }

        //////////////////// Owncloud Account Settings ////////////////////

        AccountSettingsPage {
            id: ocSettings

            trailingAction: Action {
                iconName: "document-open"
                visible: apl.windowWidth < apl.maxWidth
                text: i18n.tr("Folders")
                onTriggered: apl.addPageToNextColumn(apl.primaryPageSource, syncSettings)
            }

            onConnectClicked: {
                 testConnection(username, password, timer, serverURL, mobileData, lastSync)
            }

            accountSettings: {
                "username": accountSettings.username,
                "password": accountSettings.password,
                "timer": accountSettings.timer,
                "serverURL": accountSettings.serverURL,
                "mobileData": accountSettings.mobileData,
                "lastSync": accountSettings.lastSync
            }
        }

        ///////////////////// Sync Settings ////////////////////

        Page {
            id: syncSettings
            //property string syncFolders
            property var db
            header: PageHeader {
                id: syncHeader
                title: i18n.tr("Sync Settings")

                trailingActionBar {
                    actions: [
                        Action {
                            iconName: "add"
                            text: i18n.tr("Add")
                            onTriggered: {
                                folderListModel.append({"local":"", "remote":""})
                                syncSettings.addToDB();
                            }

                            //onTriggered: PopupUtils.open(fileDialog)
                        }
                    ]
                }
            }

            ListModel{
                id: folderListModel
                Component.onCompleted: syncSettings.loadDB()

            }

            //////////////////////////////////////////////////////////////////////////////
            ///////////////////////////// Database Functions /////////////////////////////
            //////////////////////////////////////////////////////////////////////////////

            function loadDB() {

                folderListModel.clear()

                syncSettings.db = LocalStorage.openDatabaseSync("Owncloud-Sync", "1.0", "Owncloud sync folders", 1000000);

                syncSettings.db.transaction(
                            function(tx) {
                                // Create the database if it doesn't already exist
                                tx.executeSql('CREATE TABLE IF NOT EXISTS SyncFolders(local TEXT, remote TEXT)');
                                // load all folder paths
                                var rs = tx.executeSql('SELECT * FROM SyncFolders');

                                for(var i = 0; i < rs.rows.length; i++) {

                                    folderListModel.append({"local":rs.rows.item(i).local, "remote":rs.rows.item(i).remote})

                                }
                            }
                            )
            }

            function addToDB(){
                console.log("Create database entry in Syncfolders table");
                syncSettings.db.transaction(
                            function(tx) {
                                // Create the database if it doesn't already exist
                                tx.executeSql('INSERT INTO SyncFolders VALUES("", "")');
                            }
                            )
            }

            function deleteFromDB(rowID){

                console.log("Delete entry from row " + Number(rowID + 1));
                syncSettings.db.transaction(
                            function(tx) {
                                // Delete the selected entry
                                tx.executeSql('DELETE FROM SyncFolders WHERE ROWID = (?)', [rowID+1]);
                            }
                            )
            }

            function updateDB(rowID){
                //console.log("Update entry on row " + (Number(rowID)+1));
                syncSettings.db.transaction(
                            function(tx) {
                                    tx.executeSql('UPDATE SyncFolders SET local=(?), remote=(?) WHERE ROWID = (?)',[ folderListModel.get(rowID).local, folderListModel.get(rowID).remote, rowID+1]);
                            }
                            )
            }


            Item{
                anchors{centerIn: parent}

                Icon {
                    id: addIcon
                    visible: !folderListModel.count
                    name: "add"
                    width: units.gu(4)
                    height: width
                    anchors{centerIn: parent}
                }

                Label{
                    visible: !folderListModel.count
                    text: i18n.tr("Add Folders")
                    anchors{horizontalCenter: parent.horizontalCenter; top: addIcon.bottom; topMargin: units.gu(2)}
                }
            }

            Label{
                id: localLabel
                visible: folderListModel.count
                text: i18n.tr("Local Folder:")
                horizontalAlignment: Text.AlignHCenter
                anchors{left:parent.left ;right:parent.horizontalCenter; top:syncHeader.bottom; margins: units.gu(2)}
            }

            Label{
                id: remoteLabel
                visible: folderListModel.count
                text: i18n.tr("Remote Folder:")
                horizontalAlignment: Text.AlignHCenter
                anchors{left:parent.horizontalCenter; right:parent.right; top:syncHeader.bottom; margins: units.gu(2)}
            }

            ListView {
                id: syncList
                visible: folderListModel.count
                anchors{left:parent.left; right:parent.right; top:localLabel.bottom; bottom:parent.bottom}
                clip: true
                model: folderListModel

                delegate: ListItem {
                    anchors{left:parent.left; right:parent.right}

                        TextField {
                            id: localText
                            text: folderListModel.get(index).local
                            anchors{left: parent.left; right: linkIcon.left; verticalCenter: parent.verticalCenter; margins: units.gu(1)}
                            placeholderText: i18n.tr("/home/phablet/photos")
                            onTextChanged: {
                                folderListModel.setProperty(index, "local", text);
                                syncSettings.updateDB(index);
                            }
                        }

                            Icon {
                            id: linkIcon
                            anchors{horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter}
                            width: units.gu(2)
                            height: width
                            name: "stock_link"
                        }

                        TextField {
                            id: remoteText
                            text: folderListModel.get(index).remote
                            anchors{left:linkIcon.right; right: parent.right; verticalCenter: parent.verticalCenter; margins: units.gu(1)}
                            //SlotsLayout.position: SlotsLayout.Trailing;
                            placeholderText: i18n.tr("/phone/photos")
                            onTextChanged: {
                                folderListModel.setProperty(index, "remote", text)
                                syncSettings.updateDB(index);
                            }
                        }


                    leadingActions: ListItemActions {
                        actions: [
                            Action {
                                iconName: "delete"
                                onTriggered: {
                                    console.log("Delete Action: Remove index " + index)
                                    syncSettings.deleteFromDB(index);
                                    folderListModel.remove(index, 1);

                                }
                            }
                        ]
                    }
                }
            }


            /*    FolderListModel {
                id: folderListModel
                showFiles: false
                folder: Qt.resolvedUrl("/home/phablet/");
                //nameFilters: [ "*" ]
            }
            */

            Component {
                id: fileDialog
                Dialog {
                    id: dialogue
                    text: i18n.tr("Select Directory To Sync:") //this should be title really but the text looked to big.
                    ListView {
                        anchors{ bottom: closeButton.top; bottomMargin: units.gu(1)}
                        clip: true
                        //width: 180;
                        height: units.gu(40)

                        model: folderListModel
                        delegate: ListItem {
                            height: layout.height + (divider.visible ? divider.height : 0)
                            ListItemLayout {
                                id: layout
                                title.text: model.filePath

                                Icon {
                                    name: "document-open"
                                    SlotsLayout.position: SlotsLayout.Leading;
                                    width: units.gu(2)
                                }
                            }
                        }
                    }


                    //Change this button for an inverted mouse area
                    Button {
                        id: closeButton
                        text: i18n.tr("Close")
                        color: UbuntuColors.red
                        anchors { bottom:parent.bottom; bottomMargin: units.gu(1)}
                        onClicked: PopupUtils.close(dialogue)
                    }
                }
            }
        }
    }
}
