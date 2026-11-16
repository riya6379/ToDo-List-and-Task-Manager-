#include "TaskManager.h"

#include <iostream>
#include <limits>
#include <string>

namespace {
int readInt(const std::string& prompt, int minimum, int maximum) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minimum && value <= maximum) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Invalid input. Enter a number from "
                  << minimum << " to " << maximum << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string readText(const std::string& prompt) {
    std::string value;
    do {
        std::cout << prompt;
        std::getline(std::cin, value);
        if (value.empty()) std::cout << "This field cannot be empty.\n";
    } while (value.empty());
    return value;
}

std::string readDeadline() {
    while (true) {
        std::string date = readText("Deadline (YYYY-MM-DD): ");
        if (date.size() == 10 && date[4] == '-' && date[7] == '-') {
            bool valid = true;
            for (int i = 0; i < 10; ++i) {
                if (i == 4 || i == 7) continue;
                if (date[i] < '0' || date[i] > '9') valid = false;
            }
            if (valid) return date;
        }
        std::cout << "Use the format YYYY-MM-DD, for example 2026-12-05.\n";
    }
}

void showMenu() {
    std::cout << "\n========== TO-DO LIST & TASK MANAGER ==========\n"
              << "1. Add a new task\n"
              << "2. Update a task\n"
              << "3. Mark task as completed\n"
              << "4. Delete a task\n"
              << "5. Display all tasks\n"
              << "6. Display pending tasks\n"
              << "7. Display completed tasks\n"
              << "8. Search task by keyword\n"
              << "9. Sort tasks\n"
              << "10. Show progress summary\n"
              << "11. Save tasks\n"
              << "0. Save and exit\n"
              << "================================================\n";
}
}

int main() {
    TaskManager manager;
    const std::string dataFile = "tasks.txt";

    if (manager.loadFromFile(dataFile))
        std::cout << "Saved tasks loaded successfully.\n";
    else
        std::cout << "No previous task file found. Starting a new list.\n";

    int choice;
    do {
        showMenu();
        choice = readInt("Enter your choice: ", 0, 11);

        switch (choice) {
        case 1: {
            std::string title = readText("Task title: ");
            int priority = readInt("Priority (1=High, 2=Medium-High, 3=Medium, 4=Low-Medium, 5=Low): ", 1, 5);
            std::string deadline = readDeadline();
            manager.addTask(title, priority, deadline);
            std::cout << "Task added successfully.\n";
            break;
        }
        case 2: {
            int id = readInt("Enter task ID to update: ", 1, 1000000000);
            std::string title = readText("New title: ");
            int priority = readInt("New priority (1=High to 5=Low): ", 1, 5);
            std::string deadline = readDeadline();
            if (manager.updateTask(id, title, priority, deadline))
                std::cout << "Task updated successfully.\n";
            else
                std::cout << "Task ID not found.\n";
            break;
        }
        case 3: {
            int id = readInt("Enter task ID to mark completed: ", 1, 1000000000);
            std::cout << (manager.markCompleted(id) ? "Task marked completed.\n" : "Task ID not found.\n");
            break;
        }
        case 4: {
            int id = readInt("Enter task ID to delete: ", 1, 1000000000);
            std::cout << (manager.deleteTask(id) ? "Task deleted successfully.\n" : "Task ID not found.\n");
            break;
        }
        case 5: manager.displayAll(); break;
        case 6: manager.displayPending(); break;
        case 7: manager.displayCompleted(); break;
        case 8: manager.searchByKeyword(readText("Enter keyword: ")); break;
        case 9: {
            std::cout << "\n1. Priority (high to low)\n2. Deadline (earliest first)\n3. Title (A-Z)\n";
            int sortChoice = readInt("Choose sorting method: ", 1, 3);
            if (sortChoice == 1) manager.sortByPriority();
            else if (sortChoice == 2) manager.sortByDeadline();
            else manager.sortByTitle();
            std::cout << "Tasks sorted successfully.\n";
            manager.displayAll();
            break;
        }
        case 10: manager.showSummary(); break;
        case 11:
            std::cout << (manager.saveToFile(dataFile) ? "Tasks saved successfully.\n" : "Error: could not save tasks.\n");
            break;
        case 0:
            std::cout << (manager.saveToFile(dataFile) ? "Tasks saved. Goodbye!\n" : "Could not save tasks. Goodbye!\n");
            break;
        }
    } while (choice != 0);

    return 0;
}
