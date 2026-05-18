import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

// Compound widget for picking a folder path. The text field is read-only
// (the user can still copy from it); the "Browse" button opens the
// native folder picker.
Item {
    id: root
    implicitHeight: 38

    property string path: ""
    property string browseTitle: "Choose folder"
    // NB: this signal must NOT be named `pathChanged` — that name is
    // already taken by Qt for the auto-emitted change signal of the
    // `path` property above. Qt 6.8 makes that collision a fatal QML
    // error which kills the whole component tree at engine.load().
    signal pathPicked(string newPath)

    RowLayout {
        anchors.fill: parent
        spacing: 8

        TextFieldA {
            id: field
            Layout.fillWidth: true
            text: root.path
            readOnly: true
        }
        GhostButton {
            text: i18n.t("home.browse")
            onClicked: dialog.open()
        }
    }

    FolderDialog {
        id: dialog
        title: root.browseTitle
        onAccepted: {
            const local = dialog.selectedFolder.toString()
                                  .replace(/^file:\/\/\//, "/")
                                  .replace(/^file:\/\//,  "")
            // Windows "/C:/x" — strip the leading slash.
            const fixed = /^\/[A-Za-z]:\//.test(local) ? local.substring(1) : local
            root.path = fixed
            root.pathPicked(fixed)
        }
    }
}
