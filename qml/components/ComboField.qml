import QtQuick
import QtQuick.Controls

// Themed ComboBox — flat surface, 1px border, amber border on focus.
ComboBox {
    id: combo
    implicitHeight: 38
    font.family: "Inter, Segoe UI, sans-serif"
    font.pixelSize: 13

    delegate: ItemDelegate {
        width: combo.width
        contentItem: Text {
            text: combo.textRole ? (Array.isArray(combo.model)
                                    ? modelData[combo.textRole]
                                    : model[combo.textRole])
                                 : modelData
            color: theme.textPrimary
            font: combo.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            color: hovered ? theme.surfaceHover : "transparent"
            radius: 6
        }
    }

    indicator: Item {
        x: combo.width - 22
        y: combo.height / 2 - 4
        width: 10; height: 8
        Canvas {
            anchors.fill: parent
            onPaint: {
                const ctx = getContext("2d")
                ctx.reset()
                ctx.strokeStyle = theme.textSecondary
                ctx.lineWidth = 1.5
                ctx.lineCap = "round"
                ctx.beginPath()
                ctx.moveTo(0, 0)
                ctx.lineTo(width / 2, height)
                ctx.lineTo(width, 0)
                ctx.stroke()
            }
        }
    }

    contentItem: Text {
        text: combo.displayText
        font: combo.font
        color: theme.textPrimary
        verticalAlignment: Text.AlignVCenter
        leftPadding: 12
        rightPadding: 28
        elide: Text.ElideRight
    }

    background: Rectangle {
        radius: 10
        color: theme.surface
        border.width: combo.activeFocus ? 1.5 : 1
        border.color: combo.activeFocus ? theme.accent : theme.border
    }

    popup: Popup {
        y: combo.height + 4
        width: combo.width
        padding: 4
        background: Rectangle {
            radius: 10
            color: theme.bg
            border.width: 1
            border.color: theme.border
        }
        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: combo.popup.visible ? combo.delegateModel : null
            currentIndex: combo.highlightedIndex
        }
    }
}
