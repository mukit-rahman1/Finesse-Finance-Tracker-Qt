#include "calendar_bridge.h"
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QDebug>
#include "Expense.h"
#include "supabaseclient.h"

CalendarBridge::CalendarBridge(QObject* parent)
    : QObject(parent),
    m_supabaseClient(nullptr)
{}

// MAIN addPayment (with isEssential)
void CalendarBridge::addPayment(const QString& name,
                                const QString& description,
                                double amount,
                                int startDay,
                                int endDay,
                                int priority,
                                const QString& frequency,
                                int numPayees,
                                bool isEssential)
{
    if (startDay < 1) startDay = 1;
    if (startDay > m_cal.dayCount()) startDay = m_cal.dayCount();

    Expense e(name.toStdString(),
              description.toStdString(),
              amount,
              startDay,
              endDay,
              priority,
              frequency.toStdString(),
              numPayees,
              isEssential);
    m_cal.addExpense(e);

    int expenseId = e.getId();
    emit dayChanged(startDay);

    if (endDay >= 1 && endDay <= m_cal.dayCount())
        emit dayChanged(endDay);

    emit paymentAdded(amount); // for Dashboard

    // Save to Supabase if client is configured
    if (m_supabaseClient && !m_userId.isEmpty()) {
        QVariantMap entryData;
        entryData["name"]        = name;
        entryData["description"] = description;
        entryData["amount"]      = amount;
        entryData["start_day"]   = startDay;
        // Only include end_day if it's valid (1-365), otherwise omit (database uses NULL)
        if (endDay >= 1 && endDay <= m_cal.dayCount()) {
            entryData["end_day"] = endDay;
        }
        entryData["priority"]     = priority;
        entryData["frequency"]    = frequency;
        entryData["num_payees"]   = numPayees;
        entryData["isEssential"]  = isEssential;

        // Store the expense ID so we can map it when Supabase responds
        entryData["_local_expense_id"] = expenseId;

        m_supabaseClient->createCalendarEntry(m_userId, entryData);
    }
}

QVariantList CalendarBridge::expensesForDay(int day) {
    QVariantList out;
    if (day < 1 || day > m_cal.dayCount()) return out;

    auto list = m_cal.getExpensesForDay(day);  // copy
    out.reserve(static_cast<int>(list.size()));
    for (const auto& ex : list) {
        QVariantMap m;
        m["id"]          = ex.getId();
        m["name"]        = QString::fromStdString(ex.getName());
        m["description"] = QString::fromStdString(ex.getDescription());
        m["amount"]      = ex.getAmount();
        m["priority"]    = ex.getPriority();
        m["frequency"]   = QString::fromStdString(ex.getFrequency());
        m["startDay"]    = ex.getStartDate();
        m["endDay"]      = ex.getEndDate();
        m["isEssential"] = ex.getIsEssential();
        out.push_back(m);
    }
    return out;
}

bool CalendarBridge::removePaymentById(int id) {
    double removedAmount = m_cal.removeExpenseByIdGlobalAmount(id);
    if (removedAmount > 0.0) {
        // Delete from Supabase if client is configured
        if (m_supabaseClient && m_expenseIdToSupabaseId.contains(id)) {
            int supabaseId = m_expenseIdToSupabaseId[id];
            m_supabaseClient->deleteCalendarEntry(supabaseId);
            m_expenseIdToSupabaseId.remove(id);
        }

        // notify QML
        emit paymentRemoved(removedAmount);
        emit dayChanged(m_selectedDay);
        return true;
    }
    return false;
}

int CalendarBridge::dayCount() const {
    return m_cal.dayCount();
}

int CalendarBridge::selectedDay() const { return m_selectedDay; }

void CalendarBridge::setSelectedDay(int d) {
    if (d < 1) d = 1;
    if (d > m_cal.dayCount()) d = m_cal.dayCount();
    if (m_selectedDay == d) return;
    m_selectedDay = d;
    emit selectedDayChanged();
    emit dayChanged(m_selectedDay);
}

/* ================= Supabase sync methods (from older version, merged) ================ */

void CalendarBridge::setSupabaseClient(SupabaseClient *client) {
    if (m_supabaseClient != client) {
        if (m_supabaseClient) {
            disconnect(m_supabaseClient, nullptr, this, nullptr);
        }
        m_supabaseClient = client;
        // Connect signals to handle responses from Supabase
        if (m_supabaseClient) {
            // Connect signal to handle response for fetching calendar entries
            connect(m_supabaseClient, &SupabaseClient::calendarEntriesFetched,
                    this, &CalendarBridge::onCalendarEntriesFetched);
            // Connect signal to handle response for creating a new calendar entry
            connect(m_supabaseClient, &SupabaseClient::calendarEntryCreated,
                    this, &CalendarBridge::onCalendarEntryCreated);
            // Connect signal to handle response for errors
            connect(m_supabaseClient, &SupabaseClient::calendarError,
                    this, &CalendarBridge::onCalendarError);
        }
    }
}

void CalendarBridge::setUserId(const QString &userId) {
    m_userId = userId;
}

void CalendarBridge::loadFromSupabase() {
    if (!m_supabaseClient) {
        qDebug() << "[CalendarBridge] SupabaseClient not set, cannot load calendar data";
        emit calendarDataError("SupabaseClient not configured");
        return;
    }

    if (m_userId.isEmpty()) {
        qDebug() << "[CalendarBridge] No user ID set, cannot load calendar data";
        emit calendarDataError("User not authenticated");
        return;
    }

    m_supabaseClient->fetchCalendarEntries(m_userId);
}

// Helper to clear all expenses from calendar
void CalendarBridge::clearAllExpenses() {
    // Collect all unique expense IDs from all days
    QSet<int> allExpenseIds;
    for (int day = 1; day <= m_cal.dayCount(); ++day) {
        auto expenses = m_cal.getExpensesForDay(day);
        for (const auto& expense : expenses) {
            allExpenseIds.insert(expense.getId());
        }
    }

    // Remove all expenses by ID
    for (int expenseId : allExpenseIds) {
        m_cal.removeExpenseByIdGlobal(expenseId);
    }

    qDebug() << "[CalendarBridge] Cleared" << allExpenseIds.size() << "existing expenses";
}

// Handle response for fetching calendar entries
void CalendarBridge::onCalendarEntriesFetched(const QJsonArray &entries) {
    // Clear existing calendar before loading to prevent duplicates
    clearAllExpenses();
    m_expenseIdToSupabaseId.clear();

    qDebug() << "[CalendarBridge] Loading" << entries.size() << "calendar entries from Supabase";

    for (const QJsonValue &value : entries) {
        QJsonObject entry = value.toObject();

        int supabaseId   = entry["id"].toInt();
        QString name     = entry["name"].toString();
        QString description = entry.value("description").toString(); // handles null
        double amount    = entry["amount"].toDouble();
        int startDay     = entry["start_day"].toInt();
        int endDay       = entry.value("end_day").toInt(0); // Default to 0 if null
        if (endDay < 1 || endDay > 365) endDay = 0; // Validate
        int priority     = entry.value("priority").toInt(0);
        QString frequency = entry.value("frequency").toString("one-time");
        int numPayees    = entry.value("num_payees").toInt(1);
        bool isEssential = entry.value("isEssential").toBool(false);

        // Create expense and add to calendar
        Expense e(name.toStdString(),
                  description.toStdString(),
                  amount,
                  startDay,
                  endDay,
                  priority,
                  frequency.toStdString(),
                  numPayees,
                  isEssential);

        m_cal.addExpense(e);
        int expenseId = e.getId();
        m_expenseIdToSupabaseId[expenseId] = supabaseId;
    }

    qDebug() << "[CalendarBridge] Loaded" << entries.size() << "calendar entries from Supabase";
    emit calendarDataLoaded();
    emit dayChanged(m_selectedDay); // Refresh current day view
}

void CalendarBridge::onCalendarEntryCreated(int entryId, int localExpenseId) {
    // Map the local expense ID to the Supabase entry ID
    if (localExpenseId > 0) {
        m_expenseIdToSupabaseId[localExpenseId] = entryId;
        qDebug() << "[CalendarBridge] Mapped local expense ID" << localExpenseId
                 << "to Supabase entry ID" << entryId;
    } else {
        qDebug() << "[CalendarBridge] Calendar entry created in Supabase with ID:"
                 << entryId << "(no local ID provided)";
    }
}

void CalendarBridge::onCalendarError(const QString &error) {
    qDebug() << "[CalendarBridge] Error with Supabase:" << error;
    emit calendarDataError(error);
}
