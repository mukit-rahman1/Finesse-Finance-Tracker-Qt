import QtQuick 2.15
import QtQuick.Controls 2.15
import "components"

Page {
    property StackView nav
    id: payments
    objectName: "Payments"

    // --- Month/day helper for converting UI -> day-of-year (1..365) ---
    // Non-leap year month lengths, same as Calendar.qml
    property var daysInMonth: [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

    // Convert (monthIndex 1..12, dayOfMonth) -> day-of-year (1..365)
    function dayOfYearFromMD(m, d) {
        m = Math.max(1, Math.min(12, parseInt(m, 10) || 1))
        var dim = daysInMonth[m - 1]
        d = Math.max(1, Math.min(parseInt(d, 10) || 1, dim))
        var doy = 0
        for (var i = 0; i < m - 1; ++i)
            doy += daysInMonth[i]
        return doy + d   // 1..365
    }

    // -------- Helper Functions --------
    function uncheckAllExcept(target, list) {
        for (let i = 0; i < list.length; ++i)
            if (list[i] !== target) list[i].checked = false
    }

    function frequencyString() {
        if (onetimeChkBox.checked) return "once"
        if (dailyChkBox.checked)   return "daily"
        if (weeklyChkBox.checked)  return "weekly"
        if (monthlyChkBox.checked) return "monthly"
        // fallback
        return "once"
    }

    function priorityValue() {
        // Priority from explicit High/Medium/Low:
        if (highPrioChkBox.checked)   return 3
        if (mediumPrioChkBox.checked) return 2
        if (lowPrioChkBox.checked)    return 1
        // Default to medium if none selected
        return 2
    }
    
    function isEssentialValue() {
        // Return true if Essential is checked, false if Non-Essential is checked
        // Default to false if neither is checked (shouldn't happen with validation)
        return essentialChkBox.checked
    }

    // Add Payment -> calls C++ CalendarBridge
    function addPaymentAction() {
        // Validate that Essential or Non-Essential is selected
        if (!essentialChkBox.checked && !nonEssentialChkBox.checked) {
            // console.warn("[Payments] Please select either Essential or Non-Essential")
            console.warn("[Payments] Please fill out all fields")
            return
        }
        else if (!highPrioChkBox.checked && !mediumPrioChkBox.checked && !lowPrioChkBox.checked){
            console.warn("[Payments] Please fill out all fields")
            return
        }

        else if (!onetimeChkBox.checked && !dailyChkBox.checked && !weeklyChkBox.checked && !monthlyChkBox.checked){
            console.warn("[Payments] Please fill out all fields")
            return
        }
        
        if (amountInput.acceptableInput && amountInput.text.length > 0) {
            const amt  = Number(amountInput.text)

            const name = nameInput.text || "Test Payment"
            const desc = descInput.text || "One-time"

            // Convert UI month/day -> internal day-of-year indices
            const start = dayOfYearFromMD(
                              startMonthCombo.currentIndex + 1,
                              startDaySpin.value
                          )

            const end = noEndCheck.checked
                        ? -1
                        : dayOfYearFromMD(
                              endMonthCombo.currentIndex + 1,
                              endDaySpin.value
                          )

            const prio  = priorityValue()
            const freq  = frequencyString()
            const ppl   = people.value
            const isEssential = isEssentialValue()

            user.calendar.addPayment(name, desc, amt, start, end, prio, freq, ppl, isEssential)

            // ✅ QML-safe logging (no template literals)
            console.log(
                "[Payments] Added \"" + name +
                "\" $" + amt.toFixed(2) +
                " to DOY " + start +
                ", end " + end
            )

            // reset a few fields
            amountInput.text = ""
            nameInput.text = ""
            descInput.text = ""
            amountInput.focus = true
        } else {
            amountInput.focus = true
            amountInput.selectAll()
        }
    }


    implicitWidth: 6 * 70
    implicitHeight: 13 * 70
    background: Rectangle { color: "#FFFFFF" }

    Header { id: header; x: 0; y: 0 }

    Rectangle {
        id: paymentAddContainer
        x: 0; y: 109; width: 420; height: 136; color: "#ffffff"

        Text { id: paymentAddTitle; x: 26; y: -5; text: qsTr("Add Payment"); font.pixelSize: 18; font.bold: true }

        Rectangle {
            id: nameField
            x: 50; y: 29; width: 320; height: 34
            radius: 9; border.color: "#909090"; border.width: 2; color: "#ffffff"
            TextField { anchors.fill: parent; anchors.margins: 8; hoverEnabled: false; placeholderTextColor: "#3f000000"; id: nameInput; color: "#000000"; placeholderText: "Payment name (e.g., Rent)" }
        }

        Rectangle {
            id: descField
            x: 50; y: 63; width: 320; height: 34
            radius: 9; border.color: "#909090"; border.width: 2; color: "white"
            TextField { anchors.fill: parent; anchors.margins: 8; anchors.leftMargin: 8; anchors.rightMargin: 8; anchors.topMargin: 8; anchors.bottomMargin: 8; placeholderTextColor: "#3f000000"; id: descInput; color: "#000000"; placeholderText: "Description" }
        }
    }

    Rectangle {
        id: amountField
        x: 50; y: 208; width: 320; height: 42
        color: "#ffffff"; radius: 9; border.color: "#909090"; border.width: 2
        TextField {
            id: amountInput
            anchors.fill: parent; anchors.margins: 8
            color: "#000000"; font.pixelSize: 16
            placeholderTextColor: "#3f000000"
            placeholderText: "Enter Amount..."
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            validator: DoubleValidator { bottom: 0.0 }
            Keys.onReturnPressed: addPaymentAction()
        }
    }

    Rectangle {
        id: paymentPropertiesContainer
        x: 0; y: 251; width: 420; height: 570; color: "#ffffff"

        Text { id: importanceTitle; x: 26; y: 8;  text: "Importance"; font.pixelSize: 18; font.bold: true }
        Text { id: frequencyTitle; x: 26; y: 122; text: "Frequency";  font.pixelSize: 18; font.bold: true }

        // --- Importance (Essential vs Non-essential) ---
        CheckBox {
            id: essentialChkBox
            x: 26; y: 43; width: 29; height: 26; text: ""; tristate: false; checkState: Qt.Unchecked
            onClicked: if (checked) nonEssentialChkBox.checked = false
        }
        Text { x: 61; y: 48; text: "Essential Payment"; font.pixelSize: 14 }

        CheckBox {
            id: nonEssentialChkBox
            x: 26; y: 82; width: 29; height: 26; text: ""; tristate: false; checkState: Qt.Unchecked
            onClicked: if (checked) essentialChkBox.checked = false
        }
        Text { x: 56; y: 86; text: "Non-Essential Payment"; font.pixelSize: 14 }

        // --- Frequency (mutually exclusive) ---
        CheckBox {
            id: onetimeChkBox
            x: 26; y: 160; tristate: false; checkState: Qt.Unchecked
            onClicked: if (checked) uncheckAllExcept(onetimeChkBox, [onetimeChkBox,dailyChkBox,weeklyChkBox,monthlyChkBox])
        }
        Text { x: 87; y: 160; text: "One-time"; font.pixelSize: 14 }

        CheckBox {
            id: dailyChkBox
            x: 26; y: 200; tristate: false; checkState: Qt.Unchecked
            onClicked: if (checked) uncheckAllExcept(dailyChkBox, [onetimeChkBox,dailyChkBox,weeklyChkBox,monthlyChkBox])
        }
        Text { x: 87; y: 205; text: "Daily"; font.pixelSize: 14 }

        CheckBox {
            id: weeklyChkBox
            x: 26; y: 245; tristate: false; checkState: Qt.Unchecked
            onClicked: if (checked) uncheckAllExcept(weeklyChkBox, [onetimeChkBox,dailyChkBox,weeklyChkBox,monthlyChkBox])
        }
        Text { x: 87; y: 250; text: "Weekly"; font.pixelSize: 14 }

        CheckBox {
            id: monthlyChkBox
            x: 26; y: 292; tristate: false; checkState: Qt.Unchecked
            onClicked: if (checked) uncheckAllExcept(monthlyChkBox, [onetimeChkBox,dailyChkBox,weeklyChkBox,monthlyChkBox])
        }
        Text { x: 87; y: 297; text: "Monthly"; font.pixelSize: 14 }

        // --- Start/End/People ---
        // --- Start/End/People ---
        // --- Start/End (Month + Day) + People ---
        Column {
            id: startEndDayContainer
            x: 8
            y: 382
            spacing: 8

            // ---- Start row ----
            Row {
                spacing: 8

                Label {
                    color: "#000"
                    text: "Start"
                }

                ComboBox {
                    id: startMonthCombo
                    width: 80
                    model: [ "Jan","Feb","Mar","Apr","May","Jun",
                             "Jul","Aug","Sep","Oct","Nov","Dec" ]
                    currentIndex: 0  // Jan by default

                    onCurrentIndexChanged: {
                        startDaySpin.to = daysInMonth[currentIndex]
                        if (startDaySpin.value > startDaySpin.to)
                            startDaySpin.value = startDaySpin.to
                    }
                }

                SpinBox {
                    id: startDaySpin
                    width: 100
                    from: 1
                    to: daysInMonth[startMonthCombo.currentIndex]
                    value: 1
                }
            }

            // ---- End row ----
            Row {
                spacing: 8

                Label {
                    color: "#000"
                    text: "End"
                }

                CheckBox {
                    id: noEndCheck
                    text: "None"
                    checked: true
                }

                ComboBox {
                    id: endMonthCombo
                    width: 80
                    enabled: !noEndCheck.checked
                    model: startMonthCombo.model
                    currentIndex: startMonthCombo.currentIndex

                    onCurrentIndexChanged: {
                        endDaySpin.to = daysInMonth[currentIndex]
                        if (endDaySpin.value > endDaySpin.to)
                            endDaySpin.value = endDaySpin.to
                    }
                }

                SpinBox {
                    id: endDaySpin
                    width: 100
                    enabled: !noEndCheck.checked
                    from: 1
                    to: daysInMonth[endMonthCombo.currentIndex]
                    value: startDaySpin.value
                }
            }
        }



        Row {
            id: numbOfPeopleContainer
            x: 8; y: startEndDayContainer.y + startEndDayContainer.height + 8; width: 287; height: 34; spacing: 12
            Label { color: "#000"; text: "Number of People" }
            SpinBox { id: people; from: 1; to: 10; value: 1 ;font.bold: false }
        }

        // --- Priority (mutually exclusive) ---
        Text { id: prioTitle; x: 263; y: 261; text: "Priority"; font.pixelSize: 18; font.bold: true }
        CheckBox {
            id: highPrioChkBox; x: 257; y: 300; width: 20; height: 16; text: ""
            onClicked: if (checked) { mediumPrioChkBox.checked = false; lowPrioChkBox.checked = false }
        }
        Text { id: highPrioText; x: 294; y: 300; text: "High"; font.pixelSize: 14 }

        CheckBox {
            id: mediumPrioChkBox; x: 257; y: 323; width: 20; height: 16; text: ""
            onClicked: if (checked) { highPrioChkBox.checked = false; lowPrioChkBox.checked = false }
        }
        Text { id: mediumPrioText; x: 294; y: 323; text: "Medium"; font.pixelSize: 14 }

        CheckBox {
            id: lowPrioChkBox; x: 257; y: 353; width: 20; height: 16; text: ""
            onClicked: if (checked) { highPrioChkBox.checked = false; mediumPrioChkBox.checked = false }
        }
        Text { id: lowPrioText; x: 294; y: 353; text: "Low"; font.pixelSize: 14 }

        // --- Submit button ---
        Rectangle {
            id: addPaymentBtn
            x: 100; y: 493; width: 200; height: 54
            color: "#000"; radius: 12
            Text { anchors.centerIn: parent; color: "#fff"; text: "Add Payment"; font.pixelSize: 16; font.bold: true }
            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: payments.addPaymentAction() }
        }
    }

    NavBar {
        id: navBar
        x: 0
        y: 811

        nav: payments.nav
    }


}
