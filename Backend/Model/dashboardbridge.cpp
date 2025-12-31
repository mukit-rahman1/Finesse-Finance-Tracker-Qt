#include "DashboardBridge.h"

DashboardBridge::DashboardBridge(QObject* parent)
    : QObject(parent) {}

void DashboardBridge::setTotals(double totalSpent, double remainingFunds) {
    bool emitTotal = false;
    if (m_totalSpent != totalSpent) { m_totalSpent = totalSpent; emitTotal = true; }
    if (m_remainingFunds != remainingFunds) { m_remainingFunds = remainingFunds; emitTotal = true; }
    if (emitTotal) emit totalChanged();
}

void DashboardBridge::setRecent(double amount, const QString& date) {
    bool emitRecent = false;
    if (m_recentAmount != amount) { m_recentAmount = amount; emitRecent = true; }
    if (m_recentDate != date) { m_recentDate = date; emitRecent = true; }
    if (emitRecent) emit recentChanged();
}
