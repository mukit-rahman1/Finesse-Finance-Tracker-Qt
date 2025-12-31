import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "components"

Page {
    property StackView nav
    id: calender
    implicitWidth: 6 * 70
    implicitHeight: 13 * 70
    background: Rectangle { color: "#FFFFFF" }
    objectName: "Calendar"

    // --- Alert (fixed binding loop by setting explicit size; hidden by default) ---
    Dialog {
        id: alertDialog
        title: "Alert"
        modal: true
        visible: false
        implicitWidth: 280
        implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        contentItem: Label {
            text: "This is an alert!"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            padding: 20
        }
    }

    NavBar {
        id: navBar
        x: 0
        y: 811

        nav: calender.nav
    }

    Header {
        id: header1
        x: 0
        y: 0

        Rectangle {
            id: calendarContainer
            x: 0
            y: 93
            width: 420
            height: 159
            color: "#ffffff"

            //--------------logic for spinbox---------------------
            // Convert (M,D) -> day-of-year, safely clamped
            function dayOfYearFromMD(m, d) {
                m = Math.max(1, Math.min(12, parseInt(m, 10) || 1))
                var dim = daysInMonth[m - 1]
                d = Math.max(1, Math.min(parseInt(d, 10) || 1, dim))
                var doy = 0
                for (var i = 0; i < m - 1; ++i) doy += daysInMonth[i]
                return clampDay(doy + d)
            }

            // Format DOY -> "MM/DD"
            function formatMD(doy) {
                var md = toMonthAndDay(doy)
                return (md.monthIndex + 1) + "/" + md.dayOfMonth
            }

            // Parse "M/D" or "M-D" -> DOY (fallback to current if parse fails)
            function parseMD(text, fallbackDoy) {
                var m = text.trim().match(/^(\d{1,2})\s*[\/-]\s*(\d{1,2})$/)
                return m ? dayOfYearFromMD(m[1], m[2]) : fallbackDoy
            }

            //----------------------week logic-------------------------
            // Hard coding calendar to start on Monday, January 1
            property int startWeekOn: 0
            property int firstWeekdayOfJan1: 0
            property int totalDays: daysInMonth.reduce((a, b) => a+b, 0)

            function weekdayOfYear(d){
                d = clampDay(d)
                return (firstWeekdayOfJan1 + (d-1)) % 7
            }

            function startOfWeek(d){
                d = clampDay(d)
                var weekday = weekdayOfYear(d)
                var offset = ( weekday - startWeekOn +7) % 7
                return d - offset
            }

            property int displayWeekStart: startOfWeek(user.calendar.selectedDay)

            // ---- title + week arrows (use RowLayout; no anchors on children) ----
            RowLayout {
                id: titleRow
                x: 12
                y: 8
                width: parent.width - 24
                height: 24
                spacing: 8

                Text {
                    text: "Weekly Activity"
                    font.pixelSize: 18
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                    Layout.alignment: Qt.AlignVCenter
                }

                Item { Layout.fillWidth: true } // spacer

                RowLayout {
                    spacing: 6
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    ToolButton {
                        text: "◀"
                        onClicked: {
                            user.calendar.selectedDay = calendarContainer.clampDay(user.calendar.selectedDay - 7)
                        }
                    }
                    ToolButton {
                        text: "▶"
                        onClicked: {
                            user.calendar.selectedDay = calendarContainer.clampDay(user.calendar.selectedDay + 7)
                        }
                    }
                }
            }

            // ------------------ week tiles ------------------
            readonly property var dayAbbr: ["Mo","Tu","We","Th","Fr","Sa","Su"]
            readonly property var daysInMonth: [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

            // Convert to day to month
            function clampDay(d){
                return Math.max(1, Math.min(d, totalDays));
            }

            function toMonthAndDay(d){
                d = clampDay((d))
                var month = 0
                var day = d
                var md = daysInMonth
                while (month < md.length && day > md[month]){
                    day -= md[month]
                    month++
                }
                return {monthIndex: month, dayOfMonth: day}
            }

            Repeater {
                id: week
                model: 7
                delegate: Rectangle {
                    // layout to match earlier spacing
                    width: (index === 1 ? 45 : 43)
                    height: 73
                    radius: 11
                    border.width: 1
                    border.color: "#e5e7eb"
                    x: index === 0 ? 8
                                   : index === 1 ? 63
                                                 : index === 2 ? 121
                                                               : index === 3 ? 177
                                                                             : index === 4 ? 235
                                                                                           : index === 5 ? 293
                                                                                                         : 352
                    y: 52

                    // day number for this tile
                    property int dayNumber: calendarContainer.displayWeekStart + index

                    visible: dayNumber >= 1 && dayNumber <= calendarContainer.totalDays
                    enabled: visible

                    // selected -> pale red background
                    color: user.calendar.selectedDay === dayNumber ? "#FFF0F0" : "#ffffff"

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked: if (parent.enabled) {
                                       user.calendar.selectedDay = parent.dayNumber
                                       // optional: quick peek in console
                                       const items = user.calendar.expensesForDay(parent.dayNumber)
                                       console.log(`[Calendar] Day ${parent.dayNumber} payments:`, JSON.stringify(items))
                                   }
                    }

                    // number
                    Text {
                        x: 8; y: 14
                        property var md: calendarContainer.toMonthAndDay(parent.dayNumber)
                        text: md.monthIndex+1+'/'+md.dayOfMonth
                        font.pixelSize: 11; font.bold: true
                        color: (user.calendar.selectedDay === parent.dayNumber ? "#de496e" : "#000000")
                    }

                    // weekday abbr
                    Text {
                        x: 13; y: 36
                        text: calendarContainer.dayAbbr[(calendarContainer.startWeekOn + index) % 7]
                        font.pixelSize: 12
                        color: (user.calendar.selectedDay === parent.dayNumber ? "#de496e" : "#94a3b8")
                    }
                }
            }
        }
    }

    // ------------------ payment list ------------------
    Rectangle {
        id: paymentListContainer
        x: 0
        y: 256
        width: 420
        height: 554
        color: "#ffffff"

        Text {
            id: paymentsContainerTitle
            x: 25
            y: 26
            text: "Payments"
            font.pixelSize: 16
            font.bold:
            true
        }

        Row {
            id: dayRow
            x: 25
            y: 52
            spacing: 8

            Label { text: "Day" }

            // Month dropdown: uses calendarContainer.toMonthAndDay() & daysInMonth
            ComboBox {
                id: dayMonthCombo
                width: 80
                model: [ "Jan","Feb","Mar","Apr","May","Jun",
                         "Jul","Aug","Sep","Oct","Nov","Dec" ]

                // Bind to current selectedDay
                currentIndex: calendarContainer.toMonthAndDay(
                                  user.calendar.selectedDay
                              ).monthIndex

                onCurrentIndexChanged: {
                    // Update the max day for this month
                    dayOfMonthSpin.to = calendarContainer.daysInMonth[currentIndex]
                    if (dayOfMonthSpin.value > dayOfMonthSpin.to)
                        dayOfMonthSpin.value = dayOfMonthSpin.to

                    // Compute new day-of-year and push into C++
                    var newDoy = calendarContainer.dayOfYearFromMD(
                                     currentIndex + 1,
                                     dayOfMonthSpin.value
                                 )
                    if (user.calendar.selectedDay !== newDoy)
                        user.calendar.selectedDay = newDoy
                }
            }

            // Day-of-month spinbox (1..28/29/30/31)
            SpinBox {
                id: dayOfMonthSpin
                from: 1
                to: calendarContainer.daysInMonth[dayMonthCombo.currentIndex]
                value: calendarContainer.toMonthAndDay(
                            user.calendar.selectedDay
                        ).dayOfMonth

                onValueChanged: {
                    var newDoy = calendarContainer.dayOfYearFromMD(
                                     dayMonthCombo.currentIndex + 1,
                                     value
                                 )
                    if (user.calendar.selectedDay !== newDoy)
                        user.calendar.selectedDay = newDoy
                }
            }
        }


        ListView {
            id: paymentList
            x: 25; y: 90
            width: parent.width - 50
            height: parent.height - 120
            clip: true
            model: user.calendar.expensesForDay(user.calendar.selectedDay)

            delegate: Rectangle {
                width: paymentList.width
                height: 44
                radius: 10
                border.width: 1
                border.color: "#e0e0e0"
                color: "white"

                Row {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 12

                    // modelData is the current element (a JS object/map)
                    Text { text: modelData.name + " ($" + Number(modelData.amount).toFixed(2) + ")" }
                    Text { text: "p" + modelData.priority }
                    Text { text: modelData.frequency }

                    Item { width: 1; height: 1 }

                    Button {
                        text: "Remove"
                        onClicked: {
                            // IMPORTANT: use modelData.id (not 'id', which is reserved)
                            user.calendar.removePaymentById(modelData.id)
                            // requery since model is a function result
                            paymentList.model = user.calendar.expensesForDay(user.calendar.selectedDay)
                            console.log("[Calendar] Removed id=" + modelData.id + " from Day " + user.calendar.selectedDay
                            )

                        }
                    }
                }
            }
        }


        // refresh when backend says a day changed
        Connections {
            target: user.calendar
            function onDayChanged(d) {
                paymentList.model = user.calendar.expensesForDay(d)
            }
        }

    }

}
