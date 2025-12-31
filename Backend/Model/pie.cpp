#include "pie.h"
#include "calendar_bridge.h"
#include "calendar.h"
#include "report.h"
#include "Expense.h"
#include <QVariantMap>
#include <QDebug>
#include <unordered_set>
#include <QDate>

PieChartBridge::PieChartBridge(QObject* parent) : QObject(parent) {}

void PieChartBridge::setTotalFunds(double funds) {
    if (m_totalFunds != funds) {
        m_totalFunds = funds;
        emit totalFundsChanged();
        recomputeSlices();
    }
}

void PieChartBridge::setMonthIndex(int idx)
{
    if (idx < 0 || idx > 11) return;           // 0=Jan, 11=Dec
    if (m_monthIndex == idx) return;

    m_monthIndex = idx;
    emit monthIndexChanged();
    // recompute the pie chart given the new index
    recomputeSlices();
}

void PieChartBridge::updateFromCalendar(CalendarBridge* calBridge) {
    m_calBridge = calBridge;
    recomputeSlices();
}

void PieChartBridge::recomputeSlices() {
    if (!m_calBridge) {
        qDebug() << "[PieChartBridge] No calendar bridge, clearing slices";
        m_slices.clear();
        emit slicesChanged();
        return;
    }

    // Get all unique expenses from calendar
    std::vector<Expense> allExpenses;
    std::unordered_set<int> seenIds;

    //int dayCount = m_calBridge->dayCount();
    //qDebug() << "[PieChartBridge] Processing" << dayCount << "days";

    QMap <QString, QPair<int, int>> dateMap;
    dateMap["Jan"] = qMakePair(1,31);
    dateMap["Feb"] = qMakePair(32,59);
    dateMap["Mar"] = qMakePair(60,90);
    dateMap["Apr"] = qMakePair(91, 120);
    dateMap["May"] = qMakePair(121, 151);
    dateMap["Jun"] = qMakePair(152, 181);
    dateMap["Jul"] = qMakePair(182, 212);
    dateMap["Aug"] = qMakePair(213,243);
    dateMap["Sep"] = qMakePair(244,273);
    dateMap["Oct"] = qMakePair(274,304);
    dateMap["Nov"] = qMakePair(305,334);
    dateMap["Dec"] = qMakePair(335,365);

    //QString monthKey = QDate::currentDate().toString("MMM");

    int idx = m_monthIndex;
    // in case of invalid input, set to default
    if (idx < 0 || idx > 11){
        idx = QDate::currentDate().month() -1;
    }

    static const char* monthKeys[12] = {
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
    };

    QString monthKey = monthKeys[idx];


    // Collect unique expenses from all days
    for (int d = dateMap[monthKey].first; d <= dateMap[monthKey].second; ++d) {
        auto dayExpenses = m_calBridge->expensesForDay(d);
        for (const auto& v : dayExpenses) {
            QVariantMap m = v.toMap();
            int id = m["id"].toInt();
            // Convert QVariantMap to Expense
            // Default isEssential to false if not present (for backwards compatibility)
            bool isEssential = m.contains("isEssential") ? m["isEssential"].toBool() : false;
            Expense ex(
                m["name"].toString().toStdString(),
                m["description"].toString().toStdString(),
                m["amount"].toDouble(),
                m["startDay"].toInt(),
                m["endDay"].toInt(),
                m["priority"].toInt(),
                m["frequency"].toString().toStdString(),
                1,  // numPayees - not stored in QVariantMap, defaulting to 1
                isEssential
                );
            allExpenses.push_back(ex);
        }
    }


    qDebug() << "[PieChartBridge] Found" << allExpenses.size() << "unique expenses";

    // Use report.cpp function to build pie chart data
    auto slices = buildPieChartData(m_totalFunds, allExpenses);

    qDebug() << "[PieChartBridge] Generated" << slices.size() << "slices";
    for (const auto& slice : slices) {
        qDebug() << "  -" << QString::fromStdString(slice.label) << ":" << slice.value;
    }

    // Convert to QVariantList for QML
    m_slices.clear();
    for (const auto& slice : slices) {
        QVariantMap map;
        map["label"] = QString::fromStdString(slice.label);
        map["value"] = slice.value;
        m_slices.append(map);
    }

    emit slicesChanged();
    qDebug() << "[PieChartBridge] Emitted slicesChanged signal";
}

