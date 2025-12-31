#pragma once
#include <vector>
#include "day.h"

class Day; // forward declaration

class Calendar{
    private:
        std::vector<Day> calendar_days;
    
    public:
        // constrcutor
        Calendar();
        // For future use, if we want more than one year
        Calendar(const int& years);

        void addExpense(const Expense& expense);

        void removeExpense(const Expense& expense);

        void printContents();


        int dayCount() const; //getter helper for bridge

        std::vector<Expense> getExpensesForDay(int day); //getter for a sorted vector of payments

        bool removeExpenseByIdGlobal(int id); //setter remove by id for all days

        double removeExpenseByIdGlobalAmount(int id);//helper for dashbaord to see how much to adjust total spent by

};
