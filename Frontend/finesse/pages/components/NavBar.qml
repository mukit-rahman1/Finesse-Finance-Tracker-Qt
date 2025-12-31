import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    property StackView nav   // receive StackView
    id: root
    implicitWidth: 6 * 70
    implicitHeight: 13 * 7

    background: Rectangle { color: "#FFFFFF" }

        // --------- navigation helpers ----------

    function findStackViewFromParents() {
        // check parents to find a StackView-like object
        var p = root
        while (p) {
            if (p.push && p.pop && typeof p.push === "function" && typeof p.pop === "function") {
                return p
            }
            p = p.parent
        }
        return null
    }

    function getConcreteView() {
        // preferred explicit nav passed into NavBar
        if (nav) return nav

        // try global StackView.view (works when the engine knows it)
        if (typeof StackView !== "undefined" && StackView.view) return StackView.view

        // last resort: try to find a StackView above us in the object tree
        var found = findStackViewFromParents()
        if (found) return found

        // nothing found
        return null
    }

    /*
     pushPage robustly finds the view. replaces top when depth>1,
    always passes a concrete nav param to the pushed page.
    */
    function pushPage(url, params) {
        Qt.callLater(function() {
            const view = getConcreteView()
            if (!view) { console.warn("[NavBar] pushPage: no StackView available to push", url); return }

            // ensure finalParams.nav is a concrete StackView
            var finalParams = {}
            if (params) {
                // copy all provided params
                for (var k in params) finalParams[k] = params[k]
            }
            // if nav missing or null, set to the concrete view
            if (!finalParams.hasOwnProperty("nav") || finalParams.nav === null) {
                finalParams.nav = view
            }

            if (view.depth > 1) {
                if (typeof view.replace === "function") {
                    view.replace(Qt.resolvedUrl(url), finalParams)
                } else {
                    view.pop()
                    view.push(Qt.resolvedUrl(url), finalParams)
                }
            } else {
                view.push(Qt.resolvedUrl(url), finalParams)
            }
        })
    }


        // If landing page expects nav param, pass it along

    function goHome() {
        if (!nav) {
            console.warn("NavBar: no nav; cannot go home")
            return
        }

        const item = nav.currentItem
        console.log("goHome: current item =", item.objectName)

        // already home?
        if (item && item.objectName === "LandingPage") {
            console.log("Already on LandingPage — not popping")
            return
        }

        // pop all pages back to the very first one
        while (nav.depth > 2) {
            nav.pop()
        }
    }


        function goCalendar() {
            if (!nav) {
                console.warn("NavBar: no nav; cannot push Calendar")
                return
            }

            if (nav.currentItem && nav.currentItem.objectName === "Calendar") {
                console.log("Already on Calendar — not pushing again")
                return
            }

            console.log("Navigating to Calendar")
            nav.push(Qt.resolvedUrl("../Calendar.qml"), { nav: nav })
        }


        function goPayments() {
            if (!nav) {
                console.warn("NavBar: no nav; cannot push Payments")
                return
            }

            if (nav.currentItem && nav.currentItem.objectName === "Payments") {
                console.log("Already on Payments — not pushing again")
                return
            }

            console.log("Navigating to Payments")
            nav.push(Qt.resolvedUrl("../Payments.qml"), { nav: nav })
        }


        function goReports() {
            if (!nav) {
                console.warn("NavBar: no nav; cannot push Summary")
                return
            }

            if (nav.currentItem && nav.currentItem.objectName === "Summary") {
                console.log("Already on Summary — not pushing again")
                return
            }

            console.log("Navigating to Summary")
            nav.push(Qt.resolvedUrl("../Summary.qml"), { nav: nav })
        }


        function goProfile() {
            if (!nav) {
                console.warn("NavBar: no nav; cannot push Profile")
                return
            }

            if (nav.currentItem && nav.currentItem.objectName === "ProfilePage") {
                console.log("Already on Profile — not pushing again")
                return
            }

            console.log("Navigating to Profile")
            nav.push(Qt.resolvedUrl("../ProfilePage.qml"), { nav: nav })
        }


    Image {
        id: home
        x: 32
        y: 30
        width: 32
        height: 32
        source: "../../assests/Home.svg"
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: goHome()
        }
    }

    Image {
        id: calendar
        x: 112
        y: 31
        width: 32
        height: 32
        source: "../../assests/Calendar.svg"
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: goCalendar()
        }
    }

    Image {
        id: pieChart
        x: 194
        y: 30
        width: 32
        height: 32
        source: "../../assests/Pie chart.svg"
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: goReports()
        }
    }

    Image {
        id: payments
        x: 278
        y: 30
        width: 32
        height: 32
        source: "../../assests/Transfer.svg"
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: goPayments()
        }
    }

    Rectangle {
        id: profileIcon
        x: 348
        y: 27
        width: 39
        height: 39
        radius: 99

        // The picture
        Image {
            id: profilePic
            source: user.profilePic === "../assests/JohnDoe.png" ? "../../assests/JohnDoe.png": user.profilePic
            anchors.fill: parent
            smooth: true
        }

        //The border that extends past the picture
        Rectangle {
            width: 60     // larger than 200 to extend outward
            height: 60
            anchors.centerIn: parent
            radius: width / 2
            border.color: "white"
            border.width: 10
            color: "transparent"
        }

        Rectangle {
            width: 39
            height: 39
            anchors.centerIn: parent
            radius: width / 2
            border.color: "black"
            border.width: 2
            color: "transparent"
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: goProfile()
        }
    }

    Rectangle {
        id: rectangle1
        x: 0
        y: 0
        width: 420
        height: 4
        color: "#6e6e6e"
        border.color: "#6e6e6e"
    }




}
