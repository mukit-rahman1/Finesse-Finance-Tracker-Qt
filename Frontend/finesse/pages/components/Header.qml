import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    property var nav   // receive StackView
    id: root
    implicitWidth: 6 * 70
    implicitHeight: 13 * 7

    background: Rectangle { color: "#FFFFFF" }

    // Rectangle {
    //     id: rectangle
    //     x: 352
    //     y: 24
    //     width: 44
    //     height: 44
    //     color: "#000000"
    //     radius: 99
    // }

    Image {
        id: image
        x: 152
        y: -9
        width: 116
        height: 108
        source: "../../assests/Finesse.svg"
        fillMode: Image.PreserveAspectFit
    }

    // Rectangle {
    //     id: rectangle1
    //     x: 29
    //     y: 35
    //     width: 47
    //     height: 4
    //     color: "#000000"
    //     radius: 20
    // }

    // Rectangle {
    //     id: rectangle2
    //     x: 29
    //     y: 45
    //     width: 47
    //     height: 4
    //     color: "#000000"
    //     radius: 20
    // }

    // Rectangle {
    //     id: rectangle3
    //     x: 29
    //     y: 55
    //     width: 47
    //     height: 4
    //     color: "#000000"
    //     radius: 20
    // }

    Rectangle {
        id: rectangle4
        x: 0
        y: 88
        width: 420
        height: 3
        color: "#6e6e6e"
        border.color: "#6e6e6e"
    }




}
