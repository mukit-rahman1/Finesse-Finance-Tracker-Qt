#pragma once
#include <QObject>
#include <QVariant>
#include <QJsonArray>
#include <QMap>
#include "calendar.h"

class SupabaseClient;

class CalendarBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(int selectedDay READ selectedDay WRITE setSelectedDay NOTIFY selectedDayChanged)

public:
    explicit CalendarBridge(QObject* parent = nullptr);

    Q_INVOKABLE void addPayment(const QString& name,
                                const QString& description,
                                double amount,
                                int startDay,
                                int endDay,
                                int priority,
                                const QString& frequency,
                                int numPayees = 1,
                                bool isEssential = false);

    Q_INVOKABLE QVariantList expensesForDay(int day);
    Q_INVOKABLE bool removePaymentById(int id);

    Q_INVOKABLE int selectedDay() const;
    Q_INVOKABLE void setSelectedDay(int d);

    Q_INVOKABLE int dayCount() const;

    // Supabase sync methods
    void setSupabaseClient(SupabaseClient *client);
    void setUserId(const QString &userId);
    void loadFromSupabase();

private:
    void clearAllExpenses(); // Helper to clear calendar before loading

signals:
    void selectedDayChanged();
    void dayChanged(int day);
    void paymentAdded(double amount); //for Dashboard
    void paymentRemoved(double amount); //for Dashboard
    void calendarDataLoaded();
    void calendarDataError(const QString &error);

private slots:
    void onCalendarEntriesFetched(const QJsonArray &entries);
    void onCalendarEntryCreated(int entryId, int localExpenseId);
    void onCalendarError(const QString &error);



private:
    Calendar m_cal;     // 365-day calendar
    int m_selectedDay{1};
    SupabaseClient* m_supabaseClient;
    QString m_userId;
    QMap<int, int> m_expenseIdToSupabaseId; // Maps local expense ID to Supabase entry ID
};
