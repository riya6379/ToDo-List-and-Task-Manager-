#include "TaskManager.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <utility>

TaskManager::TaskManager() : head(nullptr), nextId(1) {}

TaskManager::~TaskManager() {
    clear();
}

void TaskManager::clear() {
    while (head != nullptr) {
        Node* old = head;
        head = head->next;
        delete old;
    }
}

TaskManager::Node* TaskManager::findNode(int id) const {
    Node* current = head;
    while (current != nullptr) {
        if (current->data.id == id) return current;
        current = current->next;
    }
    return nullptr;
}

std::string TaskManager::toLower(const std::string& value) {
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return result;
}

void TaskManager::addTask(const std::string& title, int priority,
                          const std::string& deadline) {
    Task task{nextId++, title, priority, deadline, false};
    Node* newNode = new Node(task);

    if (head == nullptr) {
        head = newNode;
        return;
    }

    Node* current = head;
    while (current->next != nullptr) current = current->next;
    current->next = newNode;
}

bool TaskManager::updateTask(int id, const std::string& title, int priority,
                             const std::string& deadline) {
    Node* node = findNode(id);
    if (node == nullptr) return false;

    node->data.title = title;
    node->data.priority = priority;
    node->data.deadline = deadline;
    return true;
}

bool TaskManager::markCompleted(int id) {
    Node* node = findNode(id);
    if (node == nullptr) return false;
    node->data.completed = true;
    return true;
}

bool TaskManager::deleteTask(int id) {
    if (head == nullptr) return false;

    if (head->data.id == id) {
        Node* old = head;
        head = head->next;
        delete old;
        return true;
    }

    Node* current = head;
    while (current->next != nullptr && current->next->data.id != id)
        current = current->next;

    if (current->next == nullptr) return false;

    Node* old = current->next;
    current->next = old->next;
    delete old;
    return true;
}

// Bubble sort by swapping task data between linked-list nodes.
void TaskManager::sortByPriority() {
    if (head == nullptr) return;
    bool swapped;
    do {
        swapped = false;
        Node* current = head;
        while (current->next != nullptr) {
            if (current->data.priority > current->next->data.priority) {
                std::swap(current->data, current->next->data);
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);
}

void TaskManager::sortByDeadline() {
    if (head == nullptr) return;
    bool swapped;
    do {
        swapped = false;
        Node* current = head;
        while (current->next != nullptr) {
            // YYYY-MM-DD dates sort correctly as strings.
            if (current->data.deadline > current->next->data.deadline) {
                std::swap(current->data, current->next->data);
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);
}

void TaskManager::sortByTitle() {
    if (head == nullptr) return;
    bool swapped;
    do {
        swapped = false;
        Node* current = head;
        while (current->next != nullptr) {
            if (toLower(current->data.title) > toLower(current->next->data.title)) {
                std::swap(current->data, current->next->data);
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);
}

static void printHeader() {
    std::cout << "\n"
              << std::left << std::setw(6) << "ID"
              << std::setw(28) << "TITLE"
              << std::setw(12) << "PRIORITY"
              << std::setw(15) << "DEADLINE"
              << "STATUS\n";
    std::cout << std::string(72, '-') << '\n';
}

static void printTask(const Task& task) {
    const char* priorityText[] = {"", "High", "Medium-High", "Medium", "Low-Medium", "Low"};
    std::cout << std::left << std::setw(6) << task.id
              << std::setw(28) << task.title.substr(0, 26)
              << std::setw(12) << priorityText[task.priority]
              << std::setw(15) << task.deadline
              << (task.completed ? "Completed" : "Pending") << '\n';
}

void TaskManager::displayAll() const {
    if (head == nullptr) {
        std::cout << "\nNo tasks found.\n";
        return;
    }
    printHeader();
    for (Node* current = head; current != nullptr; current = current->next)
        printTask(current->data);
}

void TaskManager::displayPending() const {
    bool found = false;
    for (Node* current = head; current != nullptr; current = current->next) {
        if (!current->data.completed) {
            if (!found) printHeader();
            printTask(current->data);
            found = true;
        }
    }
    if (!found) std::cout << "\nNo pending tasks.\n";
}

void TaskManager::displayCompleted() const {
    bool found = false;
    for (Node* current = head; current != nullptr; current = current->next) {
        if (current->data.completed) {
            if (!found) printHeader();
            printTask(current->data);
            found = true;
        }
    }
    if (!found) std::cout << "\nNo completed tasks.\n";
}

void TaskManager::searchByKeyword(const std::string& keyword) const {
    const std::string key = toLower(keyword);
    bool found = false;

    for (Node* current = head; current != nullptr; current = current->next) {
        if (toLower(current->data.title).find(key) != std::string::npos) {
            if (!found) printHeader();
            printTask(current->data);
            found = true;
        }
    }
    if (!found) std::cout << "\nNo task matched that keyword.\n";
}

bool TaskManager::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) return false;

    for (Node* current = head; current != nullptr; current = current->next) {
        const Task& t = current->data;
        out << t.id << ' ' << std::quoted(t.title) << ' '
            << t.priority << ' ' << std::quoted(t.deadline) << ' '
            << t.completed << '\n';
    }
    return static_cast<bool>(out);
}

bool TaskManager::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return false; // A missing file is normal on the first run.

    Node* loadedHead = nullptr;
    Node* tail = nullptr;
    int largestId = 0;

    int id, priority;
    std::string title, deadline;
    bool completed;

    while (in >> id >> std::quoted(title) >> priority
              >> std::quoted(deadline) >> completed) {
        if (id <= 0 || priority < 1 || priority > 5) continue;

        Task task{id, title, priority, deadline, completed};
        Node* node = new Node(task);

        if (loadedHead == nullptr) loadedHead = node;
        else tail->next = node;

        tail = node;
        if (id > largestId) largestId = id;
    }

    clear();
    head = loadedHead;
    nextId = largestId + 1;
    return true;
}

void TaskManager::showSummary() const {
    int total = 0, completed = 0, pending = 0;
    for (Node* current = head; current != nullptr; current = current->next) {
        ++total;
        if (current->data.completed) ++completed;
        else ++pending;
    }

    std::cout << "\n========== TASK SUMMARY ==========\n"
              << "Total tasks     : " << total << '\n'
              << "Pending tasks   : " << pending << '\n'
              << "Completed tasks : " << completed << '\n';

    if (total > 0) {
        double percentage = (static_cast<double>(completed) / total) * 100.0;
        std::cout << std::fixed << std::setprecision(1)
                  << "Completion rate : " << percentage << "%\n";
    } else {
        std::cout << "Completion rate : 0.0%\n";
    }
    std::cout << "==================================\n";
}
