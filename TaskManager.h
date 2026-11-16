#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "Task.h"
#include <string>

class TaskManager {
private:
    struct Node {
        Task data;
        Node* next;

        explicit Node(const Task& task) : data(task), next(nullptr) {}
    };

    Node* head;
    int nextId;

    Node* findNode(int id) const;
    static std::string toLower(const std::string& value);
    void clear();

public:
    TaskManager();
    ~TaskManager();

    TaskManager(const TaskManager&) = delete;
    TaskManager& operator=(const TaskManager&) = delete;

    // Module 1: Task Creation
    void addTask(const std::string& title, int priority,
                 const std::string& deadline);

    // Module 2: Task Update and Deletion
    bool updateTask(int id, const std::string& title, int priority,
                    const std::string& deadline);
    bool markCompleted(int id);
    bool deleteTask(int id);

    // Module 3: Task Sorting
    void sortByPriority();
    void sortByDeadline();
    void sortByTitle();

    // Module 4: Task Display and Filter
    void displayAll() const;
    void displayPending() const;
    void displayCompleted() const;
    void searchByKeyword(const std::string& keyword) const;

    // Module 5: Data Storage and Progress Summary
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    void showSummary() const;
};

#endif
