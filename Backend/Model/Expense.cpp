//
// Created by brian on 2025-09-28.
//

#include "Expense.h"

#include <iostream>
#include "Expense.h"


int Expense::nextId = 1;

Expense::Expense(string const& name, string const& description, double amount, int startDate, int endDate, int priority, string const& frequency, const int& numPayees, bool isEssential)
{
    this->id=nextId++;
    this->name=name;
    this->description=description;
    this->amount=amount/numPayees; // divide the amount by the number of payees
    this->startDate=startDate;
    // adding end date to attribute list
    this->endDate=endDate;
    this->priority=priority;
    // adding frequency to the attribute list
    this->frequency=frequency;
    this->isEssential=isEssential;
}

int Expense::getId() const
{
    return id;
}

string Expense::getName() const
{
    return name;

}
string Expense::getDescription() const
{
    return description;
}

double Expense::getAmount() const
{
    return amount;
}

int Expense::getStartDate() const
{
    return startDate;
}

int Expense::getEndDate() const
{
    return endDate;
}

int Expense::getPriority() const
{
    return priority;
}
string Expense::getFrequency() const
{
    return frequency;
}

bool Expense::getIsEssential() const
{
    return isEssential;
}

