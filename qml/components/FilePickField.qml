import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

// Compound widget for picking a file path.  Mirrors PathField but uses
// `FileDialog` instead of `FolderDialog` and lets the caller restrict
// the dialog to specific name filters (e.g. a Netscape cookies .txt or
// an executable on Windows).
//
// Unlike PathField the text field is editable: power users routinely
// paste an absolute path directly and Tab away, and read-only mode
// would lock them out for no good reason.  Either entry method
// (typing + commit, or clicking *Browse* + native dialog) emits
// `filePicked(newPath)` so the binding on the caller stays single-shot.
Item {
    id: root
    implicitHeight: 38

    property string path: ""
    property string browseTitle: "Choose file"
    // Name filters use the Qt syntax: "Label (*.ext1 *.ext2)".  Multiple
    // entries can be supplied to give the user a category drop-down in
    // the native picker.  Default ("All files") matches PathField's
    // permissive feel.
    property var    nameFilters: [ "All files (*)" ]

    // See PathField.qml — `pathPicked` is already used by PathField, so
    // we deliberately pick a different name here to avoid coupling.
    signal filePicked(string newPath)

    function _normalize(url) {
        const local = url.toString()
                          .replace(/^file:\/\/\//, "/")
                          .replace(/^file:\/\//,  "")
        return /^\/[A-Za-z]:\//.test(local) ? local.substring(1) : local
    }

    RowLayout {
        anchors.fill: parent
        spacing: 8

        TextFieldA {
            id: field
            Layout.fillWidth: true
            text: root.path
            onEditingFinished: {
                if (text !== root.path) {
                    root.path = text
                    root.filePicked(text)
                }
            }
        }
        GhostButton {
            text: i18n.t("home.browse")
            onClicked: dialog.open()
        }
    }

    FileDialog {
        id: dialog
        title: root.browseTitle
        nameFilters: root.nameFilters
        fileMode: FileDialog.OpenFile
        onAccepted: {
            const fixed = root._normalize(dialog.selectedFile)
            root.path = fixed
            root.filePicked(fixed)
        }
    }
}
