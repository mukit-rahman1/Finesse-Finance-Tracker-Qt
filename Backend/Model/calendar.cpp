#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include "calendar.h"
#include "day.h"
#include "Expense.h"

Calendar::Calendar(){
    calendar_days.reserve(365);
    for (int i = 1; i<=365; i++){
        calendar_days.emplace_back(i); //emplace back calls the day constructor
    }
}

Calendar::Calendar(const int& years){
    calendar_days.reserve(years*365);
    for (int i = 1; i<=(years*365); i++){
        calendar_days.emplace_back(i); //emplace back calls the day constructor
    }
}

// there should a current day option, so i know when to start the payment
void Calendar::addExpense(const Expense& expense){
    // allows for 0 or negative number to indicate no end date
    int duration;
    if (expense.getEndDate() < 1){
        duration = calendar_days.size();
    }
    else{
        duration = expense.getEndDate();
    }

    std::string frequency = expense.getFrequency();

    if (frequency == "once" || frequency == "Once"){
        calendar_days[expense.getStartDate()-1].addExpenseOnce(expense);
        std::cout << "One time expense added" << std::endl;
    }
    else if (frequency == "daily" || frequency == "Daily"){
        for (int i = expense.getStartDate()-1; i<duration; i++){
            calendar_days[i].addExpenseOnce(expense);
        }
        std::cout << "Daily expense added" << std::endl;
    }
    else if (frequency == "weekly" || frequency == "Weekly"){
        for (int i = expense.getStartDate()-1; i<duration; i+=7){
            calendar_days[i].addExpenseOnce(expense);
        }
        std::cout << "Weekly expense added" << std::endl;
    }
    else if (frequency == "monthly"){
        int index = 0;
        std::vector<int> daysInMonth{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int i = expense.getStartDate()-1;
        while (index<12){
            calendar_days[i].addExpenseOnce(expense);
            i+=daysInMonth[index];
            index++;
        }
        std::cout << "Monthly expense added" << std::endl;
    }


}

void Calendar::removeExpense(const Expense& expense){
    std::string frequency = expense.getFrequency();
    if (frequency == "once"){
        calendar_days[expense.getStartDate()-1].removeExpenseOnce(expense.getId());
    }
    else if (frequency == "daily"){
        for (int i = expense.getStartDate()-1; i<calendar_days.size(); i++){
            calendar_days[i].removeExpenseOnce(expense.getId());
        }
        std::cout << "Daily expense removed" << std::endl;
    }
    else if (frequency == "weekly"){
        for (int i = expense.getStartDate()-1; i<calendar_days.size(); i+=7){
            calendar_days[i].removeExpenseOnce(expense.getId());
        }
        std::cout << "Weekly expense removed" << std::endl;
    }
    else if (frequency == "monthly"){
        for (int i = expense.getStartDate()-1; i<calendar_days.size(); i+=30){
            calendar_days[i].removeExpenseOnce(expense.getId());
        }
        std::cout << "Monthly expense removed" << std::endl;
    }


}

void Calendar::printContents(){
    for (int i = 0; i < calendar_days.size(); i++){
        if (calendar_days[i].getExpenses().size() > 0){
            calendar_days[i].sortExpenses();
        }
        std::vector<Expense> expenseList = calendar_days[i].getExpenses();
        // std::cout << "size of list: " << expenseList.size() << std::endl;
        std::cout << "Day number: " << calendar_days[i].getDayOfYear() << std::endl;
        for (std::vector<Expense>::iterator it = expenseList.begin(); it != expenseList.end(); ++it) {
            std::cout << it->getName() << ":\t" << "$" << it->getAmount() << std::endl;
        }

        std::cout << '\n' << std::endl;
    }
}

//helper getter for day count
int Calendar::dayCount() const {
    return static_cast<int>(calendar_days.size());
}

//getter sorted vector of expenses
std::vector<Expense> Calendar::getExpensesForDay(int day){
    if (day < 1 || day > static_cast<int>(calendar_days.size())) return {}; //incase error making calendar
    auto& d = calendar_days[day - 1];
    return d.getExpensesSorted();
}

//Setter for removing expense for all days
bool Calendar::removeExpenseByIdGlobal(int id) {
    bool removed = false;
    for (auto& d : calendar_days) {
        auto before = d.getExpenses().size();
        d.removeExpenseOnce(id);
        auto after  = d.getExpenses().size();
        if (after < before) removed = true; //confirm if expense got removed
    }
    return removed;
}

//Helper function for dashboard to see how much to adjust total spent by
double Calendar::removeExpenseByIdGlobalAmount(int id) {
    double totalRemoved = 0.0;
    // Iterate all days; get a copy of the day's expenses to inspect amounts,
    // then call existing removeExpenseOnce to remove by id.
    for (auto &d : calendar_days) {
        // copy current expenses to inspect
        std::vector<Expense> list = d.getExpenses();
        for (const auto &ex : list) {
            if (ex.getId() == id) {
                totalRemoved += ex.getAmount();
            }
        }
        // attempt to remove from this day (safe even if not present)
        d.removeExpenseOnce(id);
    }
    return totalRemoved;
}
