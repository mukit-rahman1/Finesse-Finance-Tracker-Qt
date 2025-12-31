/*
 * Report.h
 * Header file for expense reporting and analytics functions
 * Provides data structures and function declarations for generating
 * financial reports, pie charts, and expense summaries
 */

#ifndef REPORT_H
#define REPORT_H

#include <string>
#include <vector>
#include <unordered_map>


// Forward declaration to avoid circular includes
class Expense;

/**
 * @brief Data structure representing a slice of a pie chart
 * Contains label and value for visualization purposes
 */
struct ReportSlice {
    std::string label;    ///< Display label for the slice (e.g., "Essential", "Funds Left")
    double value;         ///< Monetary value represented by this slice
};

/**
 * @brief Calculate total amount spent across all expenses
 * @param expenses Vector of Expense objects to analyze
 * @return Total amount spent (only positive amounts are included)
 */
double computeTotalSpent(const std::vector<Expense>& expenses);

/**
 * @brief Group expenses by Essential/Non-Essential and calculate totals
 * @param expenses Vector of Expense objects to analyze
 * @return Unordered map with "Essential" or "Non-Essential" as key and total amount as value
 */
std::unordered_map<std::string, double> computeSpentByEssential(const std::vector<Expense>& expenses);

/**
 * @brief Generate pie chart data showing funds remaining and spending by Essential/Non-Essential
 * @param totalFunds Total available funds for the period
 * @param expenses Vector of Expense objects to analyze
 * @return Vector of ReportSlice objects ready for pie chart visualization
 */
std::vector<ReportSlice> buildPieChartData(double totalFunds, const std::vector<Expense>& expenses);

/**
 * @brief Create formatted text lines for each expense
 * @param expenses Vector of Expense objects to format
 * @return Vector of formatted strings containing expense details
 */
std::vector<std::string> buildExpenseLines(const std::vector<Expense>& expenses);

/**
 * @brief Print formatted expense list to standard output
 * @param expenses Vector of Expense objects to display
 */
void printExpenseList(const std::vector<Expense>& expenses);

/**
 * @brief Print pie chart data to standard output
 * @param totalFunds Total available funds for the period
 * @param expenses Vector of Expense objects to analyze
 */
void printPieChartData(double totalFunds, const std::vector<Expense>& expenses);

#endif // REPORT_H
