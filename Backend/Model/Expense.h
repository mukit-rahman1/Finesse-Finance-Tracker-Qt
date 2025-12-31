/*
// Created by brian on 2025-09-28.
*/
#ifndef ELEC376_F25_GROUP12_EXPENSE_H
#define ELEC376_F25_GROUP12_EXPENSE_H

#include <string>

using namespace std;

class Expense
{
private:
    static int nextId;
    int id;
    string name;
    string description;
    double amount;
    int startDate;
    int endDate;
    int priority;
    string frequency;
    int numPayees;
    bool isEssential;

public:
    Expense(const string& name, const string& description, double amount, int startDate, int endDate, int priority, const string& frequency, const int& numPayees, bool isEssential);

    int getId() const;
    string getName() const;
    string getDescription() const;
    double getAmount() const;
    int getStartDate() const;
    int getEndDate() const;
    int getPriority() const;
    string getFrequency() const;
    int getNumPayees() const;
    bool getIsEssential() const;
};


#endif //ELEC376_F25_GROUP12_EXPENSE_H
