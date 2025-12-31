import QtQuick 2.15
import QtQuick.Controls 2.15
import "pages"
import "pages/components"
import QtQuick.Window 2.15


ApplicationWindow {
    visible: true
    width: 6*70
    height: 13*70
    title: "Finesse"

    palette.text: "black" //make text in spinboxes black


    StackView {
        id: stack
        anchors.fill: parent

        Component.onCompleted: {
            // push LoginPage first and pass the concrete stack object
            stack.push(Qt.resolvedUrl("pages/LoginPage.qml"), { nav: stack })
        }
    }
}
