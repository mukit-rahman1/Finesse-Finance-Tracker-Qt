import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "components"

Page {
    property StackView nav
    id: root
    implicitWidth: 6 * 70
    implicitHeight: 13 * 70
    background: Rectangle { color: "#FFFFFF" }

    function handleLogin() {
        var email = usernameInput.text.trim()
        var password = passwordInput.text

        if (email === "" || password === "") {
            errorText.text = "Please enter both email and password"
            errorText.visible = true
            return
        }

        errorText.visible = false
        busyIndicator.visible = true
        auth.signInOrSignUp(email, password)
    }

    Image {
        id: ellipse161
        x: 0
        y: 253
        width: 420
        height: 657
        source: "../assests/Ellipse 161.svg"
        fillMode: Image.PreserveAspectFit

        Image {
            id: button
            x: 47
            y: 410
            source: "../assests/Button.svg"
            fillMode: Image.PreserveAspectFit

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: handleLogin()
            }
        }

        Text {
            id: usernameTitle
            x: 168
            y: 72
            color: "#a4a4a4"
            text: qsTr("Email")
            font.pixelSize: 18
            font.bold: true
        }

        Text {
            id: passwordTitle
            x: 170
            y: 173
            color: "#a4a4a4"
            text: qsTr("Password")
            font.pixelSize: 18
            font.bold: true
        }
    }

    Image {
        id: finesse
        x: 142
        y: 128
        source: "../assests/Finesse.svg"
        fillMode: Image.PreserveAspectFit
    }

    Rectangle {
        id: username

        x: 51; y: 371; width: 320; height: 34
        radius: 9; border.color: "#909090"; border.width: 2; color: "#ffffff"
        TextField {
            anchors.fill: parent
            anchors.margins: 8
            hoverEnabled: false
            placeholderTextColor: "#3f000000"
            id: usernameInput
            color: "#000000"
            placeholderText: "Enter your email"
            onAccepted: handleLogin()
        }
    }

    Rectangle {
        id: password
        x: 51
        y: 468
        width: 320
        height: 34
        color: "#ffffff"
        radius: 9
        border.color: "#909090"
        border.width: 2
        TextField {
            id: passwordInput
            color: "#000000"
            anchors.fill: parent
            anchors.margins: 8
            placeholderTextColor: "#3f000000"
            hoverEnabled: false
            echoMode: TextField.Password
            placeholderText: "Enter your password"
            onAccepted: handleLogin()
        }
    }

    // Error message text
    Text {
        id: errorText
        x: 51
        y: 520
        width: 320
        height: 30
        color: "#e63030"
        font.pixelSize: 14
        font.bold: true
        wrapMode: Text.Wrap
        visible: false
    }

    // Busy indicator
    BusyIndicator {
        id: busyIndicator
        x: 200
        y: 560
        width: 40
        height: 40
        visible: false
    }

    // Handle authentication success
    Connections {
        target: auth
        function onAuthenticationSucceeded(userId, email) {
            busyIndicator.visible = false
            // Navigate to landing page
            nav.push(Qt.resolvedUrl("LandingPage.qml"), { nav: nav })
        }
        function onAuthenticationFailed(errorMessage) {
            busyIndicator.visible = false
            errorText.text = errorMessage
            errorText.visible = true
        }
    }
}
