#ifndef GOOGLE_TASKS_API_H
#define GOOGLE_TASKS_API_H

#include <nlohmann/json.hpp>
#include <string>

#include "APIBase.h"
#include "ProfileManager.h"

class GoogleTasksAPI : public ApiBase {
   private:
    GoogleTokens googleTokens;
    std::string taskListId;
    std::string apiBase = "https://tasks.googleapis.com/tasks/v1";

    bool tryFetchTaskList();

    std::vector<nlohmann::json> getTasks(bool showCompleted, int daysBefore,
                                         int daysAfter);
    void completeTask(std::string& taskId, bool complete);
    void updateTask(std::string& taskId, std::string title,
                    std::string dueDate);
    void deleteTask(std::string& taskId);

   public:
    GoogleTasksAPI();
    void list() override;
    void list(bool showCompleted, int daysBefore, int daysAfter);
    void add() override;
    void add(std::string title, std::string dueDate);
    void edit(int daysBefore, int daysAfter);
};
#endif