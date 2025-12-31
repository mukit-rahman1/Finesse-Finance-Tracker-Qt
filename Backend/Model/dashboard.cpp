// Dashboard backend scaffold following the flow chart
// NOTE: This file provides a framework of button-style entry points that the
// front end can call. Firebase calls are represented as placeholders so the
// integration can be wired later without changing the interface.

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <iostream>

#include "calendar.h"
#include "day.h"
#include "user.h"


//functions needed: get a int value of total funds, total spent

// Forward declare the User interface we need if the header is empty
class User {
public:
    double getFunds() const;
};

// Return total available funds as an integer (rounded down)
int getTotalFunds(const User& user) {
    double funds = user.getFunds();
    if (funds < 0) return 0;
    return static_cast<int>(funds);
}

// Compute total spent for a list of expenses from the Day/Calendar model (struct Expense in day.h)
int getTotalSpent(const std::vector<Expense>& expenses) {
    double total = 0.0;
    for (const auto& e : expenses) {
        if (e.amount > 0) total += e.amount;
    }
    if (total < 0) return 0;
    return static_cast<int>(total);
}
