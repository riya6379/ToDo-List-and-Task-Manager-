#ifndef TASK_H
#define TASK_H

#include <string>

struct Task {
    int id{};
    std::string title;
    int priority{};              // 1 = High, 5 = Low
    std::string deadline;        // YYYY-MM-DD
    bool completed{false};
};

#endif
