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
    auto tokenManager = TokenManager();
    googleTokens = tokenManager.getTokens();

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
        nlohmann::json item = items.at(0);
        taskListId = item["id"].get<std::string>();
    } else if (r.status_code == 401) {
        ConfigManager::refreshConfiguration();
        GoogleTasksAPI();
    } else {
        std::cerr << "Error: " << r.status_code << std::endl;
        std::cerr << r.text << std::endl;
    }
}

void GoogleTasksAPI::list(bool showCompleted, int daysBefore, int daysAfter) {
    std::string rfcStartDay =
        TimeParse::castToRFC3339(TimeParse::getShiftedDateTime(-daysBefore));
    std::string rfcEndDay =
        TimeParse::castToRFC3339(TimeParse::getShiftedDateTime(daysAfter));

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
        // Filter out the events with status confirmed only
        std::cout << "Below are the " << items.size() << " tasks in the last "
                  << daysBefore << " days and the following " << daysAfter
                  << " days:" << std::endl;

        std::sort(items.begin(), items.end(),
                  [](const nlohmann::json& a, const nlohmann::json& b) {
                      return a["due"].get<std::string>() <
                             b["due"].get<std::string>();
                  });

        int todoCnt = 0;
        for (int i = 0; i < items.size(); i++) {
            std::string dueTime = items[i]["due"];
            std::string title = items[i]["title"];
            bool isCompleted = items[i].contains("status") &&
                               items[i]["status"] == "completed";
            std::tm time = TimeParse::parseRFC3339(dueTime);

            std::string iconColor = isCompleted ? GREEN : RED;
            std::string icon = isCompleted ? "[✔]" : "[✗]";
            if (!isCompleted) todoCnt++;

            std::cout << iconColor << icon << RESET << " "
                      << std::put_time(&time, "%Y-%m-%d") << " " << title
                      << std::endl;
        }
        std::cout << std::endl
                  << "There are " << todoCnt << " uncompleted tasks."
                  << std::endl;

    } else {
        std::cerr << "Error: " << r.status_code << std::endl;
        std::cerr << r.text << std::endl;
    }
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