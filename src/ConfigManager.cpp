#include "ConfigManager.h"

#include <fstream>
#include <iostream>

#include "GoogleEventsAPI.h"
#include "GoogleOauth.h"
#include "GoogleTasksAPI.h"
#include "GoogleTokens.h"
#include "ProfileManager.h"

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
        localCredPath = "credential.json";
    }

    if (!fileExists(localCredPath)) {
        std::cerr << "Error: Credential file not found" << std::endl;
        return;
    }
    GoogleOauth oauth(localCredPath);
    oauth.showCredential();

    std::string authorization_url = oauth.getAuthorizationUrl();

    std::cout << "\033[1;31mPlease copy the following URL and paste it into "
                 "your browser "
                 "to proceed with Google login:\033[0m\n"
              << authorization_url << std::endl;

    std::string code;
    std::cout << "\033[1;31mEnter the code: \033[0m";
    std::cin >> code;

    try {
        GoogleTokens tokens = oauth.getAccessToken(code);
        std::cout << "Access Token: " << tokens.token << std::endl;
        std::cout << "Refresh Token: " << tokens.refreshToken << std::endl;

        ProfileManager profileManager;
        profileManager.setTokens(tokens);

        std::cout << "Fetching the calendar list...\n";
        GoogleEventsAPI calendarAPI(tokens);
        std::vector<std::pair<std::string, std::string>> calendarList =
            calendarAPI.fetchCalendarList();
        profileManager.setCalendarList(calendarList);

        std::cout << "Fetching the task list...\n";
        GoogleTasksAPI tasksAPI(tokens);
        std::vector<std::pair<std::string, std::string>> taskList =
            tasksAPI.fetchTaskList();
        profileManager.setTaskList(taskList);

        std::cout << "Setting the timezone to Asia/Taipei\n";
        std::string timezone = "Asia/Taipei";
        profileManager.setTimezone(timezone);

        profileManager.saveProfile();

        std::cout << "Configuration set successfully" << std::endl;
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
