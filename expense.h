#include "string"

using namespace std;

class Expense {

public:
    double amount;
    string category;
    string description;
    string date;

    Expense(double amount, string category, string description, string date) {
        this->amount = amount;
        this->category = category;
        this->description = description;
        this->date = date;
    }
};

int partition_by_amount(vector<Expense>& expenses, int start, int end) {
    Expense pivot = expenses[end];
    int i = (start - 1);
    for (int j = start; j < end; j++) {
        if (-expenses[j].amount < -pivot.amount) {
            i++;

            swap(expenses[i], expenses[j]);
        }
    }

    swap(expenses[i + 1], expenses[end]);

    return i + 1;
}

void quick_sort_by_amount(vector<Expense>& expenses, int start, int end) {
    if (start >= end) return;

    int part = partition_by_amount(expenses, start, end);

    quick_sort_by_amount(expenses, start, part - 1);
    quick_sort_by_amount(expenses, part + 1, end);
}

int partition_by_date(vector<Expense>& expenses, int start, int end) {
    Expense pivot = expenses[end];
    int i = (start - 1);
    for (int j = start; j < end; j++) {
        if (expenses[j].date < pivot.date) {
            i++;

            swap(expenses[i], expenses[j]);
        }
    }

    swap(expenses[i + 1], expenses[end]);

    return i + 1;
}

void quick_sort_by_date(vector<Expense>& expenses, int start, int end) {
    if (start >= end) return;

    int part = partition_by_date(expenses, start, end);

    quick_sort_by_amount(expenses, start, part - 1);
    quick_sort_by_amount(expenses, part + 1, end);
}