#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>

#include "report.h"
#include "Expense.h"

// Sum total spent across all expenses (amounts <= 0 are ignored)
double computeTotalSpent(const std::vector<Expense>& expenses) {
    double total = 0.0;
    for (const auto& expense : expenses) {
        if (expense.getAmount() > 0.0) {
            total += expense.getAmount();
        }
    }
    return std::max(0.0, total);
}

// Aggregate spend by Essential/Non-Essential
std::unordered_map<std::string, double> computeSpentByEssential(const std::vector<Expense>& expenses) {
    std::unordered_map<std::string, double> spentByEssential;
    for (const auto& expense : expenses) {
        if (expense.getAmount() > 0.0) {
            std::string category = expense.getIsEssential() ? "Essential" : "Non-Essential";
            spentByEssential[category] += expense.getAmount();
        }
    }
    return spentByEssential;
}

// Build pie chart slices: funds left + Essential + Non-Essential
std::vector<ReportSlice> buildPieChartData(double totalFunds, const std::vector<Expense>& expenses) {
    const double totalSpent = computeTotalSpent(expenses);
    const double fundsLeft = std::max(0.0, totalFunds - totalSpent);

    std::vector<ReportSlice> slices;
    slices.push_back({"Funds Left", fundsLeft});

    auto byEssential = computeSpentByEssential(expenses);
    // Always include Essential and Non-Essential, even if amount is 0
    double essentialAmount = byEssential.count("Essential") > 0 ? byEssential["Essential"] : 0.0;
    double nonEssentialAmount = byEssential.count("Non-Essential") > 0 ? byEssential["Non-Essential"] : 0.0;

    // Add Essential first, then Non-Essential
    if (essentialAmount > 0.0) {
        slices.push_back({"Essential", essentialAmount});
    }
    if (nonEssentialAmount > 0.0) {
        slices.push_back({"Non-Essential", nonEssentialAmount});
    }

    return slices;
}

// Produce a simple textual list of all expenses for reporting
std::vector<std::string> buildExpenseLines(const std::vector<Expense>& expenses) {
    std::vector<std::string> lines;
    lines.reserve(expenses.size());

    for (const auto& expense : expenses) {
        std::string line = expense.getName() + " | $" + std::to_string(expense.getAmount()) +
                           " | priority=" + std::to_string(expense.getPriority()) +
                           " | id=" + std::to_string(expense.getId());
        lines.push_back(line);
    }
    return lines;
}

// Print expense list to stdout
void printExpenseList(const std::vector<Expense>& expenses) {
    auto lines = buildExpenseLines(expenses);
    std::cout << "Expenses (" << lines.size() << ")" << std::endl;
    for (const auto& line : lines) {
        std::cout << " - " << line << std::endl;
    }
}

// Print pie chart data to stdout
void printPieChartData(double totalFunds, const std::vector<Expense>& expenses) {
    auto slices = buildPieChartData(totalFunds, expenses);
    std::cout << "Pie Chart Data" << std::endl;
    for (const auto& slice : slices) {
        std::cout << " - " << slice.label << ": $" << slice.value << std::endl;
    }
}


