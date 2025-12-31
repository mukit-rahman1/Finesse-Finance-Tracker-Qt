#include "dashboardcontroller.h"

// If you want to reuse your existing helper functions later,
// just include them and call inside recompute():
// #include "dashboard.h"   // int getTotalFunds(const User&), int getTotalSpent(const std::vector<Expense>&)
// #include "user.h"

#include <algorithm>
#include <cmath>

DashboardController::DashboardController(QObject* parent)
    : QObject(parent) {}

void DashboardController::setFunds(int dollars) {
    if (dollars < 0) dollars = 0;
    if (m_totalFunds == dollars) return;
    m_totalFunds = dollars;
    recompute();
}

void DashboardController::addExpense(double amount) {
    m_amounts.push_back(amount);
    recompute();
}

void DashboardController::setExpensesFromQml(const QVariantList& list) {
    m_amounts.clear();
    m_amounts.reserve(list.size());
    for (const auto& v : list) {
        if (v.canConvert<double>()) {
            m_amounts.push_back(v.toDouble());
        } else if (v.canConvert<QVariantMap>()) {
            auto m = v.toMap();
            if (m.contains("amount")) m_amounts.push_back(m.value("amount").toDouble());
        }
    }
    recompute();
}

void DashboardController::clearExpenses() {
    if (m_amounts.isEmpty()) return;
    m_amounts.clear();
    recompute();
}

// ---- Backend wiring (use these later when you connect real models) ----

void DashboardController::setUser(User* user) {
    m_user = user;
    // If you want to pull from your helper now, uncomment when available:
    // if (m_user) m_totalFunds = std::max(0, getTotalFunds(*m_user));
    // else m_totalFunds = 0;
    // For now, just recompute with existing m_totalFunds.
    recompute();
}

void DashboardController::setExpenses(const QVector<double>& amts) {
    m_amounts = amts;
    recompute();
}

// ---- Internal ----

void DashboardController::recompute() {
    // Compute totalSpent by summing positive amounts and flooring to dollars.
    double sumPos = 0.0;
    for (double a : m_amounts) {
        if (a > 0.0) sumPos += a;
    }
    int spentDollars = static_cast<int>(std::floor(sumPos));
    if (spentDollars < 0) spentDollars = 0;

    // remaining = max(0, funds - spent)
    int remaining = m_totalFunds - spentDollars;
    if (remaining < 0) remaining = 0;

    bool changed = (spentDollars != m_totalSpent) || (remaining != m_remainingFunds);
    m_totalSpent = spentDollars;
    m_remainingFunds = remaining;

    if (changed) emit totalsChanged();
}
