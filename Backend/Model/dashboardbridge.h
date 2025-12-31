#pragma once
#include <QObject>

class DashboardBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(double totalSpent READ totalSpent NOTIFY totalChanged)
    Q_PROPERTY(double remainingFunds READ remainingFunds NOTIFY totalChanged)
    Q_PROPERTY(double recentAmount READ recentAmount NOTIFY recentChanged)
    Q_PROPERTY(QString recentDate READ recentDate NOTIFY recentChanged)

public:
    explicit DashboardBridge(QObject* parent = nullptr);

    double totalSpent() const { return m_totalSpent; }
    double remainingFunds() const { return m_remainingFunds; }
    double recentAmount() const { return m_recentAmount; }
    QString recentDate() const { return m_recentDate; }

public slots:
    // call these from payments manager / calendar bridge when data changes
    void setTotals(double totalSpent, double remainingFunds);
    void setRecent(double amount, const QString& date);

signals:
    void totalChanged();
    void recentChanged();

private:
    double m_totalSpent = 0.0;
    double m_remainingFunds = 0.0;
    double m_recentAmount = 0.0;
    QString m_recentDate = QString();
};

