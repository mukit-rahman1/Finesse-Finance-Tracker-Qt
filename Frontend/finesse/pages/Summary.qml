import QtQuick 2.15
import QtQuick.Controls 2.15
import "components"

Page {
    property StackView nav
    background: Rectangle { color: "#FFFFFF" }
    id: summary
    implicitWidth: 6 * 70
    implicitHeight: 13 * 70
    objectName: "Summary"

    Component.onCompleted: {
        console.log("Summary page loaded, pieBridge available:", typeof pieBridge !== "undefined")
        if (typeof pieBridge !== "undefined") {
            console.log("pieBridge.slices:", pieBridge.slices)
        }
    }

    // -------- Helper Functions to Extract Values from pieBridge --------
    
    // Format a number as currency string (e.g., 32.5 -> "$32.50")
    function formatCurrency(amount) {
        if (typeof amount === 'undefined' || amount === null || isNaN(amount)) {
            return "$0.00"
        }
        return "$" + amount.toFixed(2)
    }
    
    // Get Essential Payments total
    function getEssentialPayments() {
        if (!pieBridge || !pieBridge.slices) return 0.0
        
        var total = 0.0
        for (var i = 0; i < pieBridge.slices.length; i++) {
            var slice = pieBridge.slices[i]
            // Look for "Essential" label
            if (slice.label === "Essential") {
                total += slice.value
            }
        }
        return total
    }
    
    // Get Non-Essential Payments total
    function getNonEssentialPayments() {
        if (!pieBridge || !pieBridge.slices) return 0.0
        
        var total = 0.0
        for (var i = 0; i < pieBridge.slices.length; i++) {
            var slice = pieBridge.slices[i]
            // Look for "Non-Essential" label
            if (slice.label === "Non-Essential") {
                total += slice.value
            }
        }
        return total
    }
    
    // Get Monthly Budget from pieBridge
    function getMonthlyBudget() {
        if (!pieBridge) return 0.0
        return pieBridge.totalFunds || 0.0
    }

    Header {
        id: header
        x: 0
        y: 0
    }

    ComboBox {
        id: monthCombo
        x: 20
        y: header.height + 10
        width: 120

        model: [ "Jan","Feb","Mar","Apr","May","Jun",
                 "Jul","Aug","Sep","Oct","Nov","Dec" ]

        // Bind to the C++ property if available; default to current index 0
        currentIndex: pieBridge ? (pieBridge.monthIndex >= 0 ? pieBridge.monthIndex : 0) : 0

        onCurrentIndexChanged: {
            if (pieBridge) {
                pieBridge.monthIndex = currentIndex
                // slicesChanged will fire from C++, and the Canvas + labels will update
            }
        }
    }


    Rectangle {
        id: rectangle
        x: 20
        y: monthCombo.y + 40
        width: 380
        height: 658
        color: "#ffffff"
        radius: 9
        border.color: "#e0e0e0"
        border.width: 2


        Rectangle {
            id: pieChartContainer
            x: 97
            y: 84
            width: 159
            height: 159
            color: "#c0c0c0"
            radius: 99
        }

        Canvas {
            id: pieChartCanvas
            x: 103
            y: 89
            width: 148
            height: 148
            z:1

            Component.onCompleted: {
              requestPaint()  // Force initial paint
            }
            
            property var pieData: pieBridge ? (pieBridge.slices || []) : []
            property real centerX: width / 2
            property real centerY: height / 2
            property real radius: Math.min(width, height) / 2 - 5
            
            Connections {
                target: typeof pieBridge !== "undefined" ? pieBridge : null
                enabled: typeof pieBridge !== "undefined"
                function onSlicesChanged() {
                    console.log("SlicesChanged signal received, new slices:", pieBridge.slices)
                    pieChartCanvas.pieData = pieBridge.slices || []
                    pieChartCanvas.requestPaint()
                }
                function onTotalFundsChanged() {
                    pieChartCanvas.requestPaint()
                }
            }
            
            onPieDataChanged: {
                console.log("Pie data property changed, slices count:", pieData ? pieData.length : 0)
                requestPaint()
            }
            
            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                
                console.log("Painting pie chart, pieData:", pieData, "length:", pieData ? pieData.length : 0)
                
                if (!pieData || pieData.length === 0) {
                    console.log("No pie data to render")
                    return
                }
                
                // Calculate total and log all slices
                var total = 0
                console.log("All pie chart slices:")
                for (var i = 0; i < pieData.length; i++) {
                    var slice = pieData[i]
                    console.log("  Slice", i, "- Label:", slice.label, "Value:", slice.value)
                    total += slice.value
                }
                console.log("Total:", total)
                
                if (total === 0) return
                
                // Function to get color based on slice label
                function getSliceColor(label) {
                    if (label === "Funds Left") {
                        return "#000000"  // Black for total budget
                    } else if (label === "Essential") {
                        return "#0024da"  // Blue for Essential payments
                    } else if (label === "Non-Essential") {
                        return "#da0000"  // Red for Non-Essential payments
                    } else {
                        return "#808080"  // Gray for other labels
                    }
                }
                
                // Separate slices: Essential/Non-Essential slices first, then "Funds Left" last
                var expenseSlices = []
                var fundsLeftSlice = null
                
                for (var i = 0; i < pieData.length; i++) {
                    var slice = pieData[i]
                    if (slice.label === "Funds Left") {
                        fundsLeftSlice = slice
                    } else if (slice.value > 0) {
                        expenseSlices.push(slice)
                    }
                }
                
                var startAngle = -Math.PI / 2 // Start at top
                
                // Draw expense slices first (Essential and Non-Essential)
                for (var i = 0; i < expenseSlices.length; i++) {
                    var slice = expenseSlices[i]
                    var sliceAngle = (slice.value / total) * 2 * Math.PI
                    
                    console.log("Drawing expense slice:", slice.label, "angle:", sliceAngle, "start:", startAngle)
                    
                    ctx.beginPath()
                    ctx.moveTo(centerX, centerY)
                    ctx.arc(centerX, centerY, radius, startAngle, startAngle + sliceAngle)
                    ctx.closePath()
                    
                    ctx.fillStyle = getSliceColor(slice.label)
                    ctx.fill()
                    
                    startAngle += sliceAngle
                }
                
                // Draw "Funds Left" last (so it doesn't cover expense slices)
                if (fundsLeftSlice && fundsLeftSlice.value > 0) {
                    var sliceAngle = (fundsLeftSlice.value / total) * 2 * Math.PI
                    console.log("Drawing Funds Left slice, angle:", sliceAngle, "start:", startAngle)
                    
                    ctx.beginPath()
                    ctx.moveTo(centerX, centerY)
                    ctx.arc(centerX, centerY, radius, startAngle, startAngle + sliceAngle)
                    ctx.closePath()
                    
                    ctx.fillStyle = "#000000"
                    ctx.fill()
                }
            }
        }

        Text {
            id: breakdownTitle
            x: 17
            y: 21
            text: qsTr("Your Monthly Expense Breakdown")
            font.pixelSize: 14
            font.bold: true
        }

        Rectangle {
            id: rectangle1
            x: 94
            y: 359
            width: 13
            height: 14
            color: "#0024da"
            radius: 99
        }

        Rectangle {
            id: rectangle2
            x: 94
            y: 442
            width: 13
            height: 14
            color: "#da0000"
            radius: 99
        }

        Text {
            id: essentialTitle
            x: 133
            y: 359
            text: qsTr("Essential Payments")
            font.pixelSize: 12
        }

        Text {
            id: nonessentialTitle
            x: 119
            y: 441
            text: qsTr("Non-Essential Payments")
            font.pixelSize: 12
        }

        Text {
            id: essentialValue
            x: 122
            y: 380
            // Property binding: automatically updates when pieBridge.slices changes
            text: {
                if (!pieBridge || !pieBridge.slices) return "$0.00"
                var total = 0.0
                for (var i = 0; i < pieBridge.slices.length; i++) {
                    if (pieBridge.slices[i].label === "Essential") {
                        total += pieBridge.slices[i].value
                    }
                }
                return formatCurrency(total)
            }
            font.pixelSize: 32
            font.bold: true
        }

        Text {
            id: remainingTitle
            x: 120
            y: 140
            text: qsTr("ADD MONTHLY")
            font.pixelSize: 15
            font.bold: true
        }

        Text {
            id: remainingTitle2
            x: 145
            y: 160
            text: qsTr("BUDGET")
            font.pixelSize: 15
            font.bold: true
        }

        Text {
            id: nonessentialValue
            x: 119
            y: 465
            // Property binding: automatically updates when pieBridge.slices changes
            text: {
                if (!pieBridge || !pieBridge.slices) return "$0.00"
                var total = 0.0
                for (var i = 0; i < pieBridge.slices.length; i++) {
                    if (pieBridge.slices[i].label === "Non-Essential") {
                        total += pieBridge.slices[i].value
                    }
                }
                return formatCurrency(total)
            }
            font.pixelSize: 32
            font.bold: true
        }

        // Text {
        //     id: remainingValue
        //     x: 119
        //     y: 145
        //     text: qsTr("$46er")
        //     font.pixelSize: 32
        //     font.bold: true
        // }

        Rectangle {
            id: rectangle3
            x: 94
            y: 522
            width: 13
            height: 14
            color: "#000000"
            radius: 99
        }

        Text {
            id: monthlyBudgetTitle
            x: 133
            y: 522
            text: qsTr("Monthly Budget")
            font.pixelSize: 12
        }

        Text {
            id: monthlyBudgetValue
            x: 120
            y: 543
            // Property binding: automatically updates when pieBridge.totalFunds changes
            text: formatCurrency(pieBridge ? (pieBridge.totalFunds || 0.0) : 0.0)
            font.pixelSize: 32
            font.bold: true
        }



    }

    NavBar {
        id: navBar
        x: 0
        y: 811

        nav : summary.nav
    }




}
