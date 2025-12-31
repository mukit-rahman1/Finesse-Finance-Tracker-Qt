#ifndef PIE_H
#define PIE_H

#include <QObject>
#include <QVariantList>
#include <QString>

class CalendarBridge; // forward declaration

class PieChartBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList slices READ slices NOTIFY slicesChanged)
    Q_PROPERTY(double totalFunds READ totalFunds WRITE setTotalFunds NOTIFY totalFundsChanged)
    // Adding prperty for dropdown menu
    Q_PROPERTY(int monthIndex READ monthIndex WRITE setMonthIndex NOTIFY monthIndexChanged)


public:
    explicit PieChartBridge(QObject* parent = nullptr);

    QVariantList slices() const { return m_slices; }
    double totalFunds() const { return m_totalFunds; }
    void setTotalFunds(double funds);

    // Call this when expenses change
    Q_INVOKABLE void updateFromCalendar(CalendarBridge* calBridge);

    //for dropdown menu
    int monthIndex () const {return m_monthIndex;}
    Q_INVOKABLE void setMonthIndex(int idx);

signals:
    void slicesChanged();
    void totalFundsChanged();
    void monthIndexChanged();


private:
    void recomputeSlices();
    CalendarBridge* m_calBridge = nullptr;
    QVariantList m_slices;
    double m_totalFunds = 1000.0; // default
    int m_monthIndex = -1; // -1 means use current month (default)
};

#endif // PIE_H

