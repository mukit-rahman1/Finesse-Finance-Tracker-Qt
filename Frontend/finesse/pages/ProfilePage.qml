import QtQuick 6.5
import QtQuick.Controls 6.5
import QtQuick.Dialogs 6.5

import "components"

Page {
    property StackView nav

    id: profilePage
    objectName: "ProfilePage"
    title: "Profile"

    Rectangle {
        anchors.fill: parent
        color: "#FFFFFF"

        Rectangle {
            id: profile
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 30
            height: 220

            // Name column
            Column {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                spacing: 20
                anchors.margins: 30

                Text {
                    text: user.firstName
                    font.pixelSize: 40
                    font.bold: true
                    font.family: "Arial"
                    color: "black"
                }

                Text {
                    text: user.lastName
                    font.pixelSize: 40
                    font.bold: true
                    font.family: "Arial"
                    color: "black"
                }
            }

            // Circular profile picture
            Rectangle {
                width: 200
                height: 200
                anchors.right: parent.right
                anchors.margins: 20

                // The picture
                Image {
                    id: profilePic
                    source: user.profilePic !== "" ? user.profilePic : "../assests/JohnDoe.png"
                    anchors.fill: parent
                    smooth: true
                }

                // The border that extends past the picture
                Rectangle {
                    width: 280     // larger than 200 to extend outward
                    height: 280
                    anchors.centerIn: parent
                    radius: width / 2
                    border.color: "white"
                    border.width: 40
                    color: "transparent"
                }

                Rectangle {
                    width: 200
                    height: 200
                    anchors.centerIn: parent
                    radius: width / 2
                    border.color: "black"
                    border.width: 5
                    color: "transparent"
                }
            }

            // Change Name Button + Dialog
            Rectangle {
                id: changeNameBtn
                y: 250
                width: 250
                height: 50
                color: "#000000"
                radius: 30
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: setName
                    color: "#ffffff"
                    text: qsTr("Change Name")
                    font.pixelSize: 20
                    font.bold: true
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: nameDialog.open()
                }

                Dialog {
                    id: nameDialog
                    x: (profilePage.width - width*2 +90) / 2
                    y: (profilePage.height - height*2 -500) / 2
                    font.pixelSize: 20
                    font.family: "Arial"
                    title: "New Name"
                    modal: true

                    onAccepted: {
                        user.firstName = firstNameField.text
                        user.lastName = lastNameField.text
                        user.updateInSupabase()
                        nameDialog.close()
                    }
                    onRejected: nameDialog.close()


                    contentItem: Column {
                        spacing: 10
                        padding: 20

                        TextField {
                            id: firstNameField
                            placeholderText: "First Name"
                            width: 200
                        }

                        TextField {
                            id: lastNameField
                            placeholderText: "Last Name"
                            width: 200
                        }

                        Rectangle {
                            id: changeNameBtn2
                            width: 155
                            height: 37
                            color: "#000000"
                            radius: 27
                            anchors.horizontalCenter: parent.horizontalCenter

                            Text {
                                id: setName2
                                color: "#ffffff"
                                text: qsTr("Set Name")
                                font.pixelSize: 14
                                font.bold: true
                                anchors.centerIn: parent
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: nameDialog.accept()
                            }
                        }
                    }
                }
            }

            // Change Password Button + Dialog
            Rectangle {
                id: changePassBtn
                y: 325
                width: 250
                height: 50
                color: "#000000"
                radius: 30
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: setPass
                    color: "#ffffff"
                    text: qsTr("Change Password")
                    font.pixelSize: 20
                    font.bold: true
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: passDialog.open()
                }

                Dialog {
                    id: passDialog
                    x: (profilePage.width - width*2 +90) / 2
                    y: (profilePage.height - height*2 -600) / 2
                    font.pixelSize: 20
                    font.family: "Arial"
                    title: "New Password"
                    modal: true

                    property bool incorrectOldPassword: false
                    property bool newPasswordMismatch: false

                    onAccepted:
                    {
                        user.password= newPassField.text
                        passDialog.close()
                    }

                    onRejected: passDialog.close()

                    contentItem: Column {
                        spacing: 10
                        padding: 20

                        TextField {
                            id: oldPassField
                            placeholderText: "Old Password"
                            width: 200
                            echoMode: TextField.Password
                        }

                        TextField {
                            id: newPassField
                            placeholderText: "New Password"
                            width: 200
                            echoMode: TextField.Password
                        }

                        TextField {
                            id: newPassMatchField
                            placeholderText: "New Password Again"
                            width: 200
                            echoMode: TextField.Password
                        }

                        Text {
                            text: "Old Password is Incorrect!"
                            font.pixelSize: 15
                            font.bold: true
                            font.family: "Arial"
                            color: "#e63030"
                            visible: passDialog.incorrectOldPassword
                        }

                        Text {
                            text: "Passwords do not match!"
                            font.pixelSize: 15
                            font.bold: true
                            font.family: "Arial"
                            color: "#e63030"
                            visible: passDialog.newPasswordMismatch
                        }

                        Rectangle {
                            id: changePassBtn2
                            width: 155
                            height: 37
                            color: "#000000"
                            radius: 27
                            anchors.horizontalCenter: parent.horizontalCenter

                            Text {
                                id: setPass2
                                color: "#ffffff"
                                text: qsTr("Set Password")
                                font.pixelSize: 14
                                font.bold: true
                                anchors.centerIn: parent
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    if (newPassField.text === newPassMatchField.text && oldPassField.text === user.password)
                                    {
                                        passDialog.accept()
                                    }
                                    else
                                    {
                                        if (newPassField.text !== newPassMatchField.text)
                                        {
                                            passDialog.newPasswordMismatch=true
                                        }
                                        else
                                        {
                                            passDialog.newPasswordMismatch=false
                                        }
                                        if (oldPassField.text !== user.password)
                                        {
                                            passDialog.incorrectOldPassword=true
                                        }
                                        else
                                        {
                                            passDialog.incorrectOldPassword=false
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: changeProfilePic
                y: 400
                width: 250
                height: 50
                color: "#000000"
                radius: 30
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: setProfPic
                    color: "#ffffff"
                    text: qsTr("Change Profile Picture")
                    font.pixelSize: 20
                    font.bold: true
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: profDialog.open()
                }

                // ------------------- Dialog -------------------
                Dialog {
                    id: profDialog
                    x: (profilePage.width - width*2 + 90) / 2
                    y: (profilePage.height - height*2 - 600) / 2
                    font.pixelSize: 20
                    font.family: "Arial"
                    title: "Change Profile Picture"
                    modal: true
                    background: Rectangle { color: "white" }

                    property string selectedFile: ""

                    onAccepted: {
                        //console.log("Selected Image:", selectedFile)
                        profDialog.close()
                    }
                    onRejected: profDialog.close()

                    contentItem: Column {
                        spacing: 15
                        padding: 20
                        anchors.horizontalCenter: parent.horizontalCenter

                        // Display the selected image preview
                        Image {
                            id: previewImage
                            source: user.profilePic !== "" ? user.profilePic : "../assests/JohnDoe.png"
                            width: 150
                            height: 150
                            anchors.horizontalCenter: parent.horizontalCenter

                            Rectangle {
                                width: 225     // larger than 200 to extend outward
                                height: 225
                                anchors.centerIn: previewImage
                                radius: width / 2
                                border.color: "white"
                                border.width: 40
                                color: "transparent"
                            }

                            Rectangle {
                                width: 150
                                height: 150
                                anchors.centerIn: previewImage
                                radius: width / 2
                                border.color: "black"
                                border.width: 5
                                color: "transparent"
                            }
                        }

                        Text {
                            text: "Select a new profile picture:"
                            font.pixelSize: 16
                            font.bold: true
                            color: "#000000"
                        }

                        // Button to choose file
                        Rectangle {
                            width: 150
                            height: 40
                            color: "#000000"
                            radius: 20
                            anchors.horizontalCenter: parent.horizontalCenter

                            Text {
                                text: "Choose File"
                                color: "white"
                                anchors.centerIn: parent
                                font.pixelSize: 14
                                font.bold: true
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: fileDialog.open()
                            }
                        }

                        // Confirm button
                        Rectangle {
                            width: 150
                            height: 40
                            color: "#000000"
                            radius: 20
                            anchors.horizontalCenter: parent.horizontalCenter

                            Text {
                                text: "Set Profile Picture"
                                color: "white"
                                anchors.centerIn: parent
                                font.pixelSize: 14
                                font.bold: true
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: profDialog.accept()
                            }
                        }
                    }
                }

                // FileDialog (for image selection)
                FileDialog {
                    id: fileDialog
                    title: "Select a Profile Picture"
                    nameFilters: ["Image files (*.png *.jpg *.jpeg *.bmp)"]
                    onAccepted: {
                        previewImage.source = fileDialog.selectedFile
                        user.profilePic = fileDialog.selectedFile
                        user.updateInSupabase()
                    }
                }
            }

            property int userBudget: 0

            Rectangle {
                id: changeMonthlyBudget
                y: 475
                width: 250
                height: 50
                color: "#000000"
                radius: 30
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: setMonthlyBudget
                    color: "#ffffff"
                    text: qsTr("Change Monthly Budget")
                    font.pixelSize: 20
                    font.bold: true
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: budgetDialog.open()
                }

                Dialog {
                    id: budgetDialog
                    x: (profilePage.width - width*2 +90) / 2
                    y: (profilePage.height -400 - height*2 -600) / 2
                    font.pixelSize: 20
                    font.family: "Arial"
                    title: "New Monthly Budget"
                    modal: true

                    onAccepted:
                    {
                        var budgetText = monthlyBudgetText.text.trim().replace(/[^0-9.]/g, "")
                        if (budgetText === "" || isNaN(parseFloat(budgetText)))
                        {
                           user.monthlyBudget = "0"
                        }
                        else
                        {
                            user.monthlyBudget = budgetText
                        }
                        user.updateInSupabase()
                        budgetDialog.close()
                    }

                    onRejected: budgetDialog.close()

                    contentItem: Column {
                        spacing: 10
                        padding: 20

                        TextField {
                            id: monthlyBudgetText
                            placeholderText: "Monthly Budget ($)"
                            width: 200

                            validator: IntValidator { bottom: 0 } // only allows non-negative integers
                        }

                        Rectangle {
                            id: monthlyBudgetBtn
                            width: 155
                            height: 37
                            color: "#000000"
                            radius: 27
                            anchors.horizontalCenter: parent.horizontalCenter

                            Text {
                                id: monthlyBudgetBtnText
                                color: "#ffffff"
                                text: qsTr("Set Monthly Budget")
                                font.pixelSize: 14
                                font.bold: true
                                anchors.centerIn: parent
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked:
                                {
                                    budgetDialog.accept()
                                }
                            }
                        }
                    }
                }
            }

            Text {
                y: 600
                id: currentBudget
                color: "black"
                text: {
                    var budget = user.monthlyBudget
                    if (!budget || budget === "") {
                        return qsTr("Current Budget: $0")
                    }
                    return qsTr("Current Budget: $") + budget
                }
                font.pixelSize: 20
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Log Out Button
            Rectangle {
                y: 650
                width: 250
                height: 50
                color: "#e63030"
                radius: 30
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    color: "#ffffff"
                    text: qsTr("Log Out")
                    font.pixelSize: 20
                    font.bold: true
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        auth.signOut()
                        nav.clear()
                        nav.push(Qt.resolvedUrl("LoginPage.qml"), { nav: nav })
                    }
                }
            }

        }

        NavBar {
            id: navBar
            anchors.bottom: parent.bottom
            width: parent.width
            nav: profilePage.nav
        }

        Header {
            id: header
            anchors.top: parent.top
            width: parent.width
            height: 80
            nav: profilePage.nav
        }
    }
}
