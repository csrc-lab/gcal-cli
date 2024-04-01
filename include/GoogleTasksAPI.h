#ifndef GOOGLE_TASKS_API_H
#define GOOGLE_TASKS_API_H

#include <string>

#include "APIBase.h"
#include "TokenManager.h"

class GoogleTasksAPI : public ApiBase {
   private:
    GoogleTokens googleTokens;
    std::string taskListId;
    std::string apiBase = "https://tasks.googleapis.com/tasks/v1";

   public:
    GoogleTasksAPI();
    void list() override;
    void list(bool showCompleted, int daysBefore, int daysAfter);
    void add() override;
    void add(std::string title, std::string dueDate);
};
#endif