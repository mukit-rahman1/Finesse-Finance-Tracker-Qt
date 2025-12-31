#pragma once
#include <string>
#include <vector>

#include "Expense.h"

class Day{
    private:
        int dayOfYear;
        std::vector<Expense> expenses;

    public:
        // constructors
        Day(int day_of_year);
        Day(int dayOfYear, std::vector<Expense> expenses);

        // Getters
        int getDayOfYear();
        std::vector<Expense> getExpenses();
        std::vector<Expense> getExpensesSorted();

        // eventually make it to be different for recurring/onetime
        void addExpenseOnce(Expense expense);

        void removeExpenseOnce(const int& id);

        void sortExpenses();

};
