#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>

#include "day.h"
#include "Expense.h"

/* Day Class */

// Constructors
Day::Day(int dayOfYear)
    : dayOfYear(dayOfYear), expenses(std::vector<Expense>())
{}

Day::Day(int dayOfYear, std::vector<Expense> expenses)
    : dayOfYear(dayOfYear), expenses(expenses)
{}

int Day::getDayOfYear(){return dayOfYear;}

std::vector<Expense> Day::getExpenses(){return expenses;}

std::vector<Expense> Day::getExpensesSorted(){
    sortExpenses();
    return expenses;
}

void Day::addExpenseOnce(Expense expense){
    std::cout << "Adding expense: " << expense.getName() << " with ID: " << expense.getId() << std::endl;
    expenses.push_back(std::move(expense));
    std::cout << "size" << expenses.size() << std::endl;
}

void Day::removeExpenseOnce(const int& id){
    // Need to find the element that matches the one that we just removed
    // maybe we could make a static variable in the constructor that acts as an id
    auto it = std::remove_if(expenses.begin(), expenses.end(), [id](const Expense& expense) {
        return expense.getId() == id;
    });
    expenses.erase(it, expenses.end());

    std::cout << "Expense with ID " << id << " removed.\n";
}

void Day::sortExpenses(){
    sort(expenses.begin(), expenses.end(),
    [](const Expense& a, const Expense& b){
        return a.getPriority() > b.getPriority();
    });
}

    