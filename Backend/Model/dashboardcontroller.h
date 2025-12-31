#pragma once
#include <QObject>
#include <QVector>
#include <QVariantList>

class User; // optional: your real model, wire later

// Dashboard <-> QML bridge.
// - Exposes totalFunds / totalSpent / remainingFunds to QML.
// - Works TODAY without main.cpp changes by letting QML call setFunds()/addExpense().
// - Later, you can connect real C++ models via setUser()/setExpenses().
class DashboardController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int totalFunds READ totalFunds NOTIFY totalsChanged)
    Q_PROPERTY(int totalSpent READ totalSpent NOTIFY totalsChanged)
    Q_PROPERTY(int remainingFunds READ remainingFunds NOTIFY totalsChanged)

public:
    explicit DashboardController(QObject* parent = nullptr);

    // --- Properties ---
    int totalFunds() const        { return m_totalFunds; }
    int totalSpent() const        { return m_totalSpent; }
    int remainingFunds() const    { return m_remainingFunds; }

    // --- QML-friendly setters (no backend needed) ---
    // Sets available funds in whole dollars (floored internally).
    Q_INVOKABLE void setFunds(int dollars);
    // Adds an expense; positive numbers count toward "spent".
    Q_INVOKABLE void addExpense(double amount);
    // Replace all expenses from QML. Accepts:
    //   [12.99, 5, 3.5]  OR  [{amount:12.99}, {amount:5}]
    Q_INVOKABLE void setExpensesFromQml(const QVariantList& list);
    Q_INVOKABLE void clearExpenses();

    // --- Backend wiring (optional; use later) ---
    Q_INVOKABLE void setUser(User* user);                       // pulls funds from User
    Q_INVOKABLE void setExpenses(const QVector<double>& amts);  // raw amounts (+ = spent)

signals:
    void totalsChanged();

private:
    void recompute();

    // state
    int m_totalFunds   = 0;  // dollars, floored, non-negative
    int m_totalSpent   = 0;  // dollars, floored, non-negative
    int m_remainingFunds = 0;

    // if you wire a real user later
    User* m_user = nullptr;

    // expense amounts (double for convenience; positive => spent)
    QVector<double> m_amounts;
};
