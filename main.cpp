#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include "sqlite/sqlite3.h"

using namespace std;

struct Expense {
    double amount;
    string category;
    string description;
    string date;
};

class ExpenseManager {
private:
    vector<Expense> expenses;
    sqlite3* db;

    void loadExpensesFromDB() {
        const char* sql = "SELECT amount, category, description, date FROM expenses;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                Expense e;
                e.amount = sqlite3_column_double(stmt, 0);
                e.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                e.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                e.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                expenses.push_back(e);
            }
        }
        sqlite3_finalize(stmt);
    }

    void saveExpenseToDB(const Expense& expense) {
        const char* sql = "INSERT INTO expenses (amount, category, description, date) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_double(stmt, 1, expense.amount);
            sqlite3_bind_text(stmt, 2, expense.category.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, expense.description.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, expense.date.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
    }

    bool isValidDate(const string& date) const {
        regex date_regex(R"(\d{4}-\d{2}-\d{2})");
        return regex_match(date, date_regex);
    }

    bool isValidAmount(double amount) const {
        return amount > 0;
    }

public:
    ExpenseManager() {
        if (sqlite3_open("expenses.db", &db) != SQLITE_OK) {
            cerr << "Error opening database." << endl;
            exit(1);
        }

        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS expenses (
                id INTEGER PRIMARY KEY,
                amount REAL,
                category TEXT,
                description TEXT,
                date DATETIME
            );
        )";

        if (sqlite3_exec(db, sql, nullptr, nullptr, nullptr) != SQLITE_OK) {
            cerr << "Error while creating the table." << endl;
            exit(1);
        }

        loadExpensesFromDB();
    }

    ~ExpenseManager() {
        sqlite3_close(db);
    }

    void addExpense(double amount, const string& category, const string& description, const string& date) {
        if (!isValidAmount(amount)) {
            cout << "Invalid amount. It must be a positive number." << endl;
            return;
        }

        if (!isValidDate(date)) {
            cout << "Invalid date format. Please use YYYY-MM-DD." << endl;
            return;
        }

        Expense expense{amount, category, description, date};
        expenses.push_back(expense);
        saveExpenseToDB(expense);
    }

    void viewExpenses() const {
        for (const auto& expense : expenses) {
            cout << "Amount: " << expense.amount
                 << ", Category: " << expense.category
                 << ", Description: " << expense.description
                 << ", Date: " << expense.date << endl;
        }
    }

    void viewTotalExpenses() const {
        double total = 0;
        for (const auto& expense : expenses) {
            total += expense.amount;
        }
        cout << "Total Expenses: " << total << endl;
    }

    void searchByCategory(const string& category) const {
        for (const auto& expense : expenses) {
            if (expense.category == category) {
                cout << "Amount: " << expense.amount
                     << ", Description: " << expense.description
                     << ", Date: " << expense.date << endl;
            }
        }
    }

    void sortExpensesByAmount() {
        sort(expenses.begin(), expenses.end(), [](const Expense& a, const Expense& b) {
            return a.amount < b.amount;
        });
        cout << "Expenses sorted by amount." << endl;
    }

    void sortExpensesByDate() {
        sort(expenses.begin(), expenses.end(), [](const Expense& a, const Expense& b) {
            return a.date < b.date;
        });
        cout << "Expenses sorted by date." << endl;
    }
};

int main() {
    auto* manager = new ExpenseManager();
    int choice = -1;

    do {
        cout << "\nExpense Manager\n";
        cout << "1. Add Expense\n";
        cout << "2. View All Expenses\n";
        cout << "3. View Total Expenses\n";
        cout << "4. Search by Category\n";
        cout << "5. Sort Expenses by Amount\n";
        cout << "6. Sort Expenses by Date\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                double amount;
                string category, description, date;
                cout << "Enter amount: ";
                cin >> amount;
                cin.ignore();
                cout << "Enter category: ";
                getline(cin, category);
                cout << "Enter description: ";
                getline(cin, description);
                cout << "Enter date (YYYY-MM-DD): ";
                getline(cin, date);
                manager->addExpense(amount, category, description, date);
                break;
            }
            case 2:
                manager->viewExpenses();
                break;
            case 3:
                manager->viewTotalExpenses();
                break;
            case 4: {
                string category;
                cin.ignore();
                cout << "Enter category: ";
                getline(cin, category);
                manager->searchByCategory(category);
                break;
            }
            case 5:
                manager->sortExpensesByAmount();
                break;
            case 6:
                manager->sortExpensesByDate();
                break;
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    delete manager;

    return 0;
}
