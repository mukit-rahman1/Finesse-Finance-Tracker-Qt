#pragma once
#include <QObject>
#include <QList>

class PaymentsManager : public QObject {
    Q_OBJECT
signals:
    void paymentAdded(double amount); //for Dashboard
public:
    explicit PaymentsManager(QObject *parent = nullptr);

public slots:
    void addPayment(double amount);
    void printAll();

public:
    Q_INVOKABLE double total() const;

private:
    QList<double> m_amounts;
};
