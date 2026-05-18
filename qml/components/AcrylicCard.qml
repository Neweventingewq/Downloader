import QtQuick

// A "frosted card" — semi-transparent surface with a 1px border and an
// inner top highlight. The amber side-stripe is optional and gives the
// queue cards their visual identity; pass `accentStripe: true` to enable.
Rectangle {
    id: card
    color: theme.surface
    radius: 14
    border.width: 1
    border.color: theme.border

    property bool accentStripe: false
    property bool elevated: false

    // Top highlight
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 1
        height: 1
        radius: 1
        color: theme.snow ? Qt.rgba(0, 0, 0, 0.05)
                          : Qt.rgba(1, 1, 1, 0.05)
    }

    // Left accent stripe
    Rectangle {
        visible: card.accentStripe
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 1
        width: 3
        radius: 2
        color: theme.accent
    }

    // Soft drop shadow approximation (one extra rect, slightly larger
    // and below — cheap, no Graphical Effects dependency required).
    Rectangle {
        visible: card.elevated && !theme.snow
        anchors.fill: parent
        anchors.topMargin: 4
        z: -1
        radius: parent.radius
        color: "transparent"
        border.width: 1
        border.color: Qt.rgba(0, 0, 0, 0.18)
    }
}
