#include "Expense.h"
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

vector<Expense*> allExpenses;

void addExpense(const string& name, const string& description, double amount, int startDate, int endDate, int priority, const string& reccurance);
void removeExpense(int id);
bool printExpense(int id);
void printAllExpenses();
void sortExpenses();
bool compareByPriority(Expense* a, Expense* b);

int main() {



    
    // addExpense("Concert Ticket", "Live concert event", 150.0, 120, 377, 2, "Monthly");
    // addExpense("Netflix", "Streaming subscription", 14.99, 45, 76, 3, "Weekly");
    // addExpense("Gym Membership", "Weekly gym fee", 10.0, 300, 0, 1, "Once");

    // printAllExpenses();

    return 0;
}
// These functions are present in the calendar class, they do not need to be in main
// void addExpense(const string& name, const string& description, double amount, int startDate, int endDate, int priority, const string& reccurance)
// {
//     if (reccurance=="Once") {
//         allExpenses.push_back(new Expense(name, description, amount, startDate, priority));
//     }
//     else if (reccurance=="Daily")
//     {
//         for(int i=startDate;i<=365&&i<=endDate;i++)
//         {
//             allExpenses.push_back(new Expense(name, description, amount, i, priority));
//         }
//     }
//     else if (reccurance=="Weekly")
//     {
//         for(int i=startDate;i<=365&&i<=endDate;i+=7)
//         {
//             allExpenses.push_back(new Expense(name, description, amount, i, priority));
//         }
//     }
//     else if (reccurance=="Monthly")
//     {
//         for(int i=startDate;i<=365&&i<=endDate;i+=31)
//         {
//             allExpenses.push_back(new Expense(name, description, amount, i, priority));
//         }
//     }
// }

// void removeExpense(int id)
// {
//     if (allExpenses.empty())
//     {
//         cout << "Empty List\n";
//         return;
//     }
//     for (int i = 0; i < allExpenses.size(); i++)
//     {
//         if (allExpenses[i]->getId() == id)
//         {
//             cout << "Removed: " << allExpenses[i]->getName() << " ("<< allExpenses[i]->getDescription() << ")\n";
//             allExpenses.erase(allExpenses.begin() + i);
//             return;
//         }
//     }
//     cout << "Expense with ID " << id << " not found.\n";
// }

// bool printExpense(int id)
// {
//     if (allExpenses.empty())
//     {
//         cout << "Empty List\n";
//         return false;
//     }
//     for (int i = 0; i < allExpenses.size(); i++)
//     {
//         if (allExpenses[i]->getId() == id)
//         {
//             cout << "Expense: " << allExpenses[i]->getName() << " ("<< allExpenses[i]->getDescription() << ") Start: "<< allExpenses[i]->getStartDate() << " ID:"<< allExpenses[i]->getId() << "\n";
//             return true;
//         }
//     }
//     cout << "Expense with ID " << id << " not found.\n";
//     return false;
// }

// void printAllExpenses()
// {
//     sortExpenses();
//     for (Expense* expense : allExpenses)
//     {
//         printExpense(expense->getId());
//     }
// }


// void sortExpenses()
// {
//     sort(allExpenses.begin(), allExpenses.end(), compareByPriority);
// }

// bool compareByPriority(Expense* a, Expense* b)
// {
//     return a->getPriority() > b->getPriority();  // Higher priority first
// }