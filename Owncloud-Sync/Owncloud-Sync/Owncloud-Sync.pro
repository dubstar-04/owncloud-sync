TEMPLATE = aux
TARGET = Owncloud-Sync

RESOURCES += Owncloud-Sync.qrc

#QML_FILES += $$files(*.qml,true) \
#             $$files(*.js,true)

CONF_FILES +=  Owncloud-Sync.apparmor \
               Owncloud-Sync.png

AP_TEST_FILES += tests/autopilot/run \
                 $$files(tests/*.py,true)

OTHER_FILES += $${CONF_FILES} \
               $${QML_FILES} \
               $${AP_TEST_FILES} \
               Owncloud-Sync.desktop


#specify where the qml/js files are installed to
qml_files.path = /Owncloud-Sync
qml_files.files += *.qml #$${QML_FILES}

#specify where the ui qml/js files are installed to
ui_files.path = /Owncloud-Sync/ui
ui_files.files += ui/*.qml

#specify where the component qml/js files are installed to
component_files.path = /Owncloud-Sync/components
component_files.files += components/*.qml

#specify where the config files are installed to
config_files.path = /Owncloud-Sync
config_files.files += $${CONF_FILES}

owncloud_files.path = /Owncloud-Sync/lib/arm-linux-gnueabihf/bin
owncloud_files.files += lib/arm-linux-gnueabihf/bin/*

lib_files.path = /Owncloud-Sync/lib/arm-linux-gnueabihf/lib
lib_files.files += lib/arm-linux-gnueabihf/lib/*

#install the desktop file, a translated version is 
#automatically created in the build directory
desktop_file.path = /Owncloud-Sync
desktop_file.files = $$OUT_PWD/Owncloud-Sync.desktop
desktop_file.CONFIG += no_check_exist

INSTALLS+=config_files qml_files desktop_file owncloud_files lib_files ui_files component_files

DISTFILES +=

