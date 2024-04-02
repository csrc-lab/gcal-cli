#include "GoogleTasksAPI.h"

#include <cpr/cpr.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

#include "ConfigManager.h"
#include "utils/TimeParse.h"

const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string RESET = "\033[0m";

GoogleTasksAPI::GoogleTasksAPI() {
    ProfileManager profileManager;
    googleTokens = profileManager.getTokensFromFile();

    if (!tryFetchTaskList()) {
        ConfigManager::refreshConfiguration();
        googleTokens = profileManager.getTokensFromFile();
        if (!tryFetchTaskList()) {
            throw std::runtime_error(
                "Failed to fetch task list after refreshing token.");
        }
    }
}

bool GoogleTasksAPI::tryFetchTaskList() {
    std::string apiUrl = apiBase + "/users/@me/lists";
    cpr::Response r =
        cpr::Get(cpr::Url{apiUrl},
                 cpr::Header{{"Authorization", "Bearer " + googleTokens.token},
                             {"Accept", "application/json"}},
                 cpr::Parameters{{"maxResults", "1"}});
    if (r.status_code == 200) {
        nlohmann::json j = nlohmann::json::parse(r.text);
        std::vector<nlohmann::json> items =
            j["items"].get<std::vector<nlohmann::json>>();
        if (!items.empty()) {
            nlohmann::json item = items.at(0);
            taskListId = item["id"].get<std::string>();
            return true;
        }
    }
    return false;
}

std::vector<nlohmann::json> GoogleTasksAPI::getTasks(bool showCompleted,
                                                     int daysBefore,
                                                     int daysAfter) {
    std::string rfcStartDay =
        TimeParse::castToRFC3339(TimeParse::getShiftedDateTime(-daysBefore));
    std::string rfcEndDay =
        TimeParse::castToRFC3339(TimeParse::getShiftedDateTime(daysAfter + 1));

    std::string apiUrl = apiBase + "/lists/" + taskListId + "/tasks";

    cpr::Response r = cpr::Get(
        cpr::Url{apiUrl},
        cpr::Header{{"Authorization", "Bearer " + googleTokens.token},
                    {"Accept", "application/json"}},
        cpr::Parameters{{"maxResults", "100"},
                        {"dueMin", rfcStartDay},
                        {"dueMax", rfcEndDay},
                        {"showDeleted", "false"},
                        {"showCompleted", showCompleted ? "true" : "false"},
                        {"showHidden", showCompleted ? "true" : "false"}});
    if (r.status_code == 200) {
        nlohmann::json j = nlohmann::json::parse(r.text);
        std::vector<nlohmann::json> items =
            j["items"].get<std::vector<nlohmann::json>>();

        std::sort(items.begin(), items.end(),
                  [](const nlohmann::json& a, const nlohmann::json& b) {
                      return a["due"].get<std::string>() <
                             b["due"].get<std::string>();
                  });
        return items;
    } else {
        std::cerr << "Error: " << r.status_code << std::endl;
        std::cerr << r.text << std::endl;
        std::vector<nlohmann::json> items;
        return items;
    }
}

void GoogleTasksAPI::list(bool showCompleted, int daysBefore, int daysAfter) {
    std::vector<nlohmann::json> tasks =
        getTasks(showCompleted, daysBefore, daysAfter);
    std::cout << "Below are the " << tasks.size() << " tasks in the last "
              << daysBefore << " days and the following " << daysAfter
              << " days:" << std::endl;

    int todoCnt = 0;
    for (int i = 0; i < tasks.size(); i++) {
        std::string dueTime = tasks[i]["due"];
        std::string title = tasks[i]["title"];
        bool isCompleted =
            tasks[i].contains("status") && tasks[i]["status"] == "completed";
        std::tm time = TimeParse::parseRFC3339(dueTime);

        std::string iconColor = isCompleted ? GREEN : RED;
        std::string icon = isCompleted ? "[V]" : "[X]";
        if (!isCompleted) todoCnt++;

        std::cout << iconColor << icon << RESET << " "
                  << std::put_time(&time, "%Y-%m-%d") << " " << title
                  << std::endl;
    }
    std::cout << std::endl
              << "There are " << todoCnt << " uncompleted tasks." << std::endl;
}

void GoogleTasksAPI::list() { list(false, 7, 7); }

void GoogleTasksAPI::add(std::string title, std::string dueDate) {
    if (title.empty()) {
        std::cout << "Enter the title of the task: ";
        std::getline(std::cin, title);
    }

    if (dueDate.empty()) {
        std::cout << "Enter the due date (YYYY-MM-DD): ";
        std::getline(std::cin, dueDate);
    }

    dueDate +=
        "T00:00:00Z";  // Convert to full RFC3339 format with a default time

    nlohmann::json taskData = {{"title", title}, {"due", dueDate}};

    std::string apiUrl =
        "https://tasks.googleapis.com/tasks/v1/lists/" + taskListId + "/tasks";
    cpr::Response r =
        cpr::Post(cpr::Url{apiUrl},
                  cpr::Header{{"Authorization", "Bearer " + googleTokens.token},
                              {"Accept", "application/json"},
                              {"Content-Type", "application/json"}},
                  cpr::Body{taskData.dump()},
                  cpr::Timeout{10000});  // 10 seconds timeout for the request

    if (r.status_code == 200 || r.status_code == 201) {
        std::cout << "Task created successfully!\n";
    } else {
        std::cerr << "Failed to create task. Status code: " << r.status_code
                  << "\n";
        std::cerr << "Response: " << r.text << "\n";
    }
}

void GoogleTasksAPI::add() { add("", ""); }

void GoogleTasksAPI::edit(int daysBefore, int daysAfter) {
    std::vector<nlohmann::json> tasks = getTasks(true, daysBefore, daysAfter);
    if (tasks.empty()) {
        std::cout << "No tasks found in the specified range." << std::endl;
        return;
    }

    // print the task and status with id
    for (int i = 0; i < tasks.size(); i++) {
        std::string dueTime = tasks[i]["due"];
        std::string title = tasks[i]["title"];
        bool isCompleted =
            tasks[i].contains("status") && tasks[i]["status"] == "completed";
        std::tm time = TimeParse::parseRFC3339(dueTime);

        std::string iconColor = isCompleted ? GREEN : RED;
        std::string icon = isCompleted ? "[V]" : "[X]";

        std::cout << std::setw(2) << i + 1 << ". " << iconColor << icon << RESET
                  << " " << std::put_time(&time, "%Y-%m-%d") << " " << title
                  << std::endl;
    }

    std::cout << std::endl << "Enter the number of the task you want to edit: ";
    int taskIndex;
    std::cin >> taskIndex;
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

    if (taskIndex < 1 || taskIndex > tasks.size()) {
        std::cout << "Invalid task number." << std::endl;
        return;
    }

    const auto& task = tasks[taskIndex - 1];

    std::cout << std::endl
              << "Select an action: \n"
              << "1. Complete task\n"
              << "2. Undo task\n"
              << "3. Update title\n"
              << "4. Update due date\n"
              << "5. Delete task\n"
              << "Enter your choice: ";
    int action;
    std::cin >> action;
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

    std::string taskId = task["id"].get<std::string>();
    std::string title, dueDate;
    try {
        switch (action) {
            case 1:
                completeTask(taskId, true);
                break;
            case 2:
                completeTask(taskId, false);
                break;
            case 3:
                std::cout << "Enter the new title of the task: ";
                std::getline(std::cin, title);

                if (!title.empty()) {
                    updateTask(taskId, title, task["due"]);
                }
                break;
            case 4:
                std::cout << "Enter the due date (YYYY-MM-DD): ";
                std::getline(std::cin, dueDate);

                if (!dueDate.empty()) {
                    dueDate += "T00:00:00Z";
                    updateTask(taskId, task["title"], dueDate);
                }
                break;
            case 5:
                deleteTask(taskId);
                break;
            default:
                std::cout << "Invalid action." << std::endl;
                return;
        }
        std::cout << std::endl << "Task updated successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void GoogleTasksAPI::completeTask(std::string& taskId, bool complete) {
    nlohmann::json patchData;
    patchData["status"] = complete ? "completed" : "needsAction";
    if (complete) {
        patchData["completed"] =
            TimeParse::castToRFC3339(TimeParse::getCurrentDateTime());
    }

    std::string apiUrl = apiBase + "/lists/" + taskListId + "/tasks/" + taskId;
    auto response = cpr::Patch(
        cpr::Url{apiUrl},
        cpr::Header{{"Authorization", "Bearer " + googleTokens.token},
                    {"Content-Type", "application/json"}},
        cpr::Body{patchData.dump()},
        cpr::Timeout{10000});  // Adjust the URL and token as necessary

    if (response.status_code != 200) {
        throw std::runtime_error("Failed to update task status. HTTP Status: " +
                                 std::to_string(response.status_code));
    }
}

void GoogleTasksAPI::updateTask(std::string& taskId, std::string title,
                                std::string dueDate) {
    nlohmann::json patchData = {{"title", title}, {"due", dueDate}};

    std::string apiUrl = apiBase + "/lists/" + taskListId + "/tasks/" + taskId;
    auto response = cpr::Patch(
        cpr::Url{apiUrl},
        cpr::Header{{"Authorization", "Bearer " + googleTokens.token},
                    {"Content-Type", "application/json"}},
        cpr::Body{patchData.dump()}, cpr::Timeout{10000});

    if (response.status_code != 200) {
        throw std::runtime_error("Failed to update task. HTTP Status: " +
                                 std::to_string(response.status_code));
    }
}

void GoogleTasksAPI::deleteTask(std::string& taskId) {
    std::string apiUrl = apiBase + "/lists/" + taskListId + "/tasks/" + taskId;
    auto response = cpr::Delete(
        cpr::Url{apiUrl},
        cpr::Header{{"Authorization", "Bearer " + googleTokens.token}},
        cpr::Timeout{10000});

    if (response.status_code != 204) {
        throw std::runtime_error("Failed to delete task. HTTP Status: " +
                                 std::to_string(response.status_code));
    }
};
