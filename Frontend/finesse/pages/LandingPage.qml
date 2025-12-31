// LandingPage.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Basic 2.15
import "components"


Page {
    id: root
    property StackView nav
    objectName: "LandingPage"

    // when the nav property changes, forward it immediately to internal navBar
    onNavChanged: {
        if (nav && navBar) {
            console.log("[LandingPage] onNavChanged: forwarding nav to navBar")
            navBar.nav = nav
        } else {
            console.log("[LandingPage] onNavChanged: nav or navBar not available yet:", nav, navBar)
        }
    }

    background: Rectangle { color: "#ffffff" }

    function stack() {        // helper
        return StackView.view || nav || null
    }

    // ---- data fallbacks (overridden automatically if `dashboard` exists) ----
    property real fallbackTotalSpent: 45.90
    property int  fallbackRemaining: 405
    property real fallbackRecent: 4.99
    property string fallbackDateLabel: "Jan 1st, 2025"

    function dollars(x, digits) {
        if (x === undefined || x === null) return "0"
        if (digits === undefined) digits = 2
        return Number(x).toFixed(digits)
    }

    // if a C++/QML object named `dashboard` exists, use it; else fallbacks
    readonly property bool hasDashboard: (typeof dashboard !== "undefined" && dashboard !== null)
    readonly property string totalSpentText:
        "$" + Number(hasDashboard ? dashboard.totalSpent : fallbackTotalSpent).toFixed(2)
    readonly property string remainingFundsText:
        "$" + Number(hasDashboard ? dashboard.remainingFunds : fallbackRemaining).toFixed(2)

    readonly property string recentText:
        "Recent: $" + Number(hasDashboard ? dashboard.recentAmount : fallbackRecent).toFixed(2)

    readonly property string dateLabel:
        hasDashboard && dashboard.recentDate ? dashboard.recentDate : fallbackDateLabel


    // ---- layout metrics ----
    readonly property int pagePadding: 16
    readonly property int cardRadius: 14
    readonly property color cardBorder: "#e5e5e5"
    readonly property int vGap: 14
    readonly property int hGap: 12

    // // ---- header (logo, avatar dot) ----
    // Rectangle {
    //     id: header
    //     anchors {
    //         top: parent.top
    //         left: parent.left
    //         right: parent.right
    //     }
    //     height: 64
    //     color: "transparent"

    //     // "Finesse" logo: "Fin" green, "esse" black
    //     Row {
    //         anchors.horizontalCenter: parent.horizontalCenter
    //         anchors.verticalCenter: parent.verticalCenter
    //         spacing: 0
    //         Text {
    //             text: "Fin"
    //             color: "#2ea043"      // green
    //             font.pixelSize: 28; font.bold: true
    //         }
    //         Text {
    //             text: "esse"
    //             color: "#000000"
    //             font.pixelSize: 28; font.bold: true
    //         }
    //     }

    //     // // avatar dot
    //     // Rectangle {
    //     //     id: avatar
    //     //     width: 22; height: 22; radius: 11
    //     //     color: "#000000"
    //     //     anchors.right: parent.right
    //     //     anchors.rightMargin: pagePadding
    //     //     anchors.verticalCenter: parent.verticalCenter
    //     // }
    // }

    Header {
        id: header
        anchors.top: parent.top
        width: parent.width
        height: 80
        nav: root.nav
    }

    // ---- main scroll area ----
    Flickable {
        id: scroll
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: navBar.top
        }
        contentWidth: width
        contentHeight: contentCol.implicitHeight + pagePadding
        clip: true

        Column {
            id: contentCol
            width: parent.width
            spacing: vGap
            anchors.margins: pagePadding
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: pagePadding

            // --- two stat cards (Total Spent | Remaining Funds) ---
            Row {
                width: parent.width
                spacing: hGap
                Rectangle {
                    id: totalSpentCard
                    width: (parent.width - pagePadding*2 - hGap) / 2 + pagePadding // keeps good look on narrow widths
                    height: 88
                    radius: cardRadius
                    color: "#ffffff"
                    border.color: cardBorder; border.width: 1

                    Column {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 6
                        Text { text: "Total Spent This Month"; color: "#000000"
                            font.pixelSize: 14; font.bold: true }
                        Text { text: totalSpentText; color: "#000000"
                            font.pixelSize: 26; font.bold: true }
                    }
                }
                Rectangle {
                    id: remainingCard
                    width: (parent.width - pagePadding*2 - hGap) / 2 + pagePadding
                    height: 88
                    radius: cardRadius
                    color: "#ffffff"
                    border.color: cardBorder; border.width: 1

                    Column {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 6
                        Text { text: "Remaining Funds"; color: "#000000"
                            font.pixelSize: 14; font.bold: true }
                        Text { text: remainingFundsText; color: "#000000"
                            font.pixelSize: 26; font.bold: true }
                    }
                }
            }

            // --- card: View Calendar ---
            Rectangle {
                width: parent.width - pagePadding*2
                height: 142
                radius: cardRadius
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#ffffff"
                border.color: cardBorder; border.width: 1

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10
                    Text {
                        text: "View Calendar"
                        font.pixelSize: 15; font.bold: true
                        color: "#000000"
                        horizontalAlignment: Text.AlignHCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: dateLabel
                        font.pixelSize: 30; font.bold: true
                        color: "#000000"
                        horizontalAlignment: Text.AlignHCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Rectangle {
                        id: btnCal
                        width: 150; height: 36; radius: 18
                        color: "#000000"
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            anchors.centerIn: parent
                            text: "Go to Calendar"
                            color: "#ffffff"
                            font.pixelSize: 14; font.bold: true
                        }
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (root.nav) root.nav.push(Qt.resolvedUrl("Calendar.qml"), { nav: root.nav })
                            }
                        }
                    }
                }
            }

            // --- card: Add or Remove Payments ---
            Rectangle {
                width: parent.width - pagePadding*2
                height: 142
                radius: cardRadius
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#ffffff"
                border.color: cardBorder; border.width: 1

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10
                    Text {
                        text: "Add or Remove Payments"
                        font.pixelSize: 15; font.bold: true
                        color: "#000000"
                        horizontalAlignment: Text.AlignHCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: recentText
                        font.pixelSize: 26; font.bold: true
                        color: "#000000"
                        horizontalAlignment: Text.AlignHCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Rectangle {
                        id: btnPay
                        width: 150; height: 36; radius: 18
                        color: "#000000"
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            anchors.centerIn: parent
                            text: "Go to Payments"
                            color: "#ffffff"
                            font.pixelSize: 14; font.bold: true
                        }
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (root.nav) root.nav.push(Qt.resolvedUrl("Payments.qml"), { nav: root.nav })
                            }
                        }
                    }
                }
            }

            // --- card: View your Expense Report ---
            Rectangle {
                width: parent.width - pagePadding*2
                height: 130
                radius: cardRadius
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#ffffff"
                border.color: cardBorder; border.width: 1

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10
                    Text {
                        text: "View your Expense Report"
                        font.pixelSize: 15; font.bold: true
                        color: "#000000"
                        horizontalAlignment: Text.AlignHCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Rectangle {
                        id: btnReport
                        width: 150; height: 36; radius: 18
                        color: "#000000"
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            anchors.centerIn: parent
                            text: "Go to Summary"
                            color: "#ffffff"
                            font.pixelSize: 14; font.bold: true
                        }
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (root.nav) root.nav.push(Qt.resolvedUrl("Summary.qml"), { nav: root.nav })
                            }
                        }
                    }
                }
            }
        }
    }

    // ---- bottom nav bar ----
    NavBar {
        id: navBar
        x: 0
        y: parent.height - implicitHeight

        // Component.onCompleted: {
        //     console.log("LandingPage Component.onCompleted — nav:", nav, "StackView.view:", StackView.view)
        //     // If nav was passed, assign once. or else try to grab StackView.view shortly after.
        //     if (nav) {
        //         navBar.nav = nav
        //         console.log("[LandingPage] assigned nav to NavBar (passed parameter)")
        //     } else {
        //         Qt.callLater(function() {
        //             if (StackView.view) {
        //                 // assign the real StackView view as nav
        //                 nav = StackView.view
        //                 navBar.nav = nav
        //                 console.log("[LandingPage] captured StackView.view and assigned to NavBar")
        //             } else {
        //                 console.warn("[LandingPage] still no nav; StackView.view not available")
        //             }
        //         })
        //     }
        // }
        nav: root.nav
    }
    Component.onCompleted: {
        console.log("LandingPage: recentAmount =", dashboard.recentAmount,
                    " recentDate =", dashboard.recentDate)
    }
}


