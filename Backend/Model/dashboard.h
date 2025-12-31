#pragma once

#include <vector>
#include "day.h" // for struct Expense

class User; // forward declaration

int getTotalFunds(const User& user);
int getTotalSpent(const std::vector<Expense>& expenses);

