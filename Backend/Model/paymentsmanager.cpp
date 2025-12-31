#include "paymentsmanager.h"
#include <QDebug>
#include <cmath>

PaymentsManager::PaymentsManager(QObject *parent) : QObject(parent) {}

void PaymentsManager::addPayment(double amount) {
    if (std::isnan(amount)) return;
    m_amounts.append(amount);                   //m_amounts is temporary vector. will use database later
    emit paymentAdded(amount); //Signal Dashboard
    qDebug() << "[Payments] Added:" << amount;
}

void PaymentsManager::printAll() {
    qDebug() << "-------- Payments (amounts) --------";
    double sum = 0.0;
    for (int i = 0; i < m_amounts.size(); ++i) {
        qDebug() << i << ":" << m_amounts[i];
        sum += m_amounts[i];
    }
    qDebug() << "Total =" << sum;
    qDebug() << "------------------------------------";
}

double PaymentsManager::total() const {
    double s = 0.0;
    for (double a : m_amounts) s += a;
    return s;
}
