#include "ConfigManager.h"

#include <fstream>
#include <iostream>

#include "GoogleEventsAPI.h"
#include "GoogleOauth.h"
#include "GoogleTasksAPI.h"
#include "GoogleTokens.h"
#include "ProfileManager.h"
#include "utils/inquirer.h"

bool fileExists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

void ConfigManager::showConfiguration() {
    // Implement the logic to show configuration
    std::cout << "Show the current configuration" << std::endl;
    ProfileManager profileManager;
    std::string token = profileManager.getToken();
    std::cout << "Access Token: " << token << std::endl;

    std::string refreshToken = profileManager.getRefreshToken();
    std::cout << "Refresh Token: " << refreshToken << std::endl;

    std::vector<std::pair<std::string, std::string>> calendarList =
        profileManager.getCalendarList();
    for (auto calendar : calendarList) {
        std::cout << "Calendar ID: " << calendar.first
                  << ", Calendar Name: " << calendar.second << std::endl;
    }

    std::vector<std::pair<std::string, std::string>> taskList =
        profileManager.getTaskList();
    for (auto task : taskList) {
        std::cout << "Task ID: " << task.first << ", Task Name: " << task.second
                  << std::endl;
    }

    std::string timezone = profileManager.getTimezone();
    std::cout << "Timezone: " << timezone << std::endl;
}

void ConfigManager::setConfiguration(const std::string& credPath) {
    std::string localCredPath = credPath;
    if (localCredPath.empty()) {
        auto inquirer = alx::Inquirer("credential_path");
        localCredPath =
            inquirer
                .add_question(
                    {"input", "Enter the path to the Google credential file"})
                .ask();
    }

    if (!fileExists(localCredPath)) {
        std::cerr << "Error: Credential file not found" << std::endl;
        return;
    }
    GoogleOauth oauth(localCredPath);

    std::string authorization_url = oauth.getAuthorizationUrl();

    std::cout << "\033[1;31mPlease copy the following URL and paste it into "
                 "your browser "
                 "to proceed with Google login:\033[0m\n"
              << authorization_url << std::endl;

    std::string code;
    auto inquirer = alx::Inquirer("login_code");
    code = inquirer.add_question({"input", "Enter the code from the browser"})
               .ask();

    try {
        GoogleTokens tokens = oauth.getAccessToken(code);
        ProfileManager profileManager;
        profileManager.setTokens(tokens);

        GoogleEventsAPI calendarAPI(tokens);
        std::vector<std::pair<std::string, std::string>> tempCalendarList =
            calendarAPI.fetchCalendarList();
        std::vector<std::pair<std::string, std::string>> calendarList;
        inquirer = alx::Inquirer("calendar_list");
        for (auto calendar : tempCalendarList) {
            std::string question;
            question = "Do you want to add calendar " + calendar.second + "?";
            std::string ans =
                inquirer.add_question({"confirm", question, alx::Type::yesNo})
                    .ask();
            if (ans == "yes") {
                calendarList.push_back(calendar);
            }
        }
        profileManager.setCalendarList(calendarList);

        std::cout << "Fetching the task list...\n";
        GoogleTasksAPI tasksAPI(tokens);
        std::vector<std::pair<std::string, std::string>> tempTaskList =
            tasksAPI.fetchTaskList();
        std::vector<std::pair<std::string, std::string>> taskList;
        inquirer = alx::Inquirer("task_list");
        for (auto task : tempTaskList) {
            std::string question;
            question = "Do you want to add task " + task.second + "?";
            std::string ans =
                inquirer.add_question({"confirm", question, alx::Type::yesNo})
                    .ask();
            if (ans == "yes") {
                taskList.push_back(task);
            }
        }
        profileManager.setTaskList(taskList);

        inquirer = alx::Inquirer("timezone");
        // std::string timezone = "Asia/Taipei";
        std::string timezone =
            inquirer
                .add_question(
                    {"type", "What is your timezone?",
                     std::vector<std::string>{
                         "Asia/Taipei", "UTC", "America/New_York",
                         "Europe/London", "Asia/Tokyo", "Australia/Sydney"}})
                .ask();
        profileManager.setTimezone(timezone);

        profileManager.saveProfile();

        std::cout << "Congratulations! Configuration set successfully"
                  << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}

void ConfigManager::refreshConfiguration() {
    ProfileManager profileManager;
    GoogleTokens tokens = profileManager.getTokens();
    GoogleOauth oauth(tokens);
    try {
        GoogleTokens newTokens = oauth.refreshTokens();

        profileManager.setTokens(newTokens);
        profileManager.saveProfile();

        std::cout << "Configuration refreshed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}
