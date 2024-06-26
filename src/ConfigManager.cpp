#include "ConfigManager.h"

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>

#include "GoogleEventsAPI.h"
#include "GoogleOauth.h"
#include "GoogleTasksAPI.h"
#include "GoogleTokens.h"
#include "ProfileManager.h"
#include "utils/httplib.h"
#include "utils/inquirer.h"

bool fileExists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

void ConfigManager::showConfiguration() {
    ProfileManager profileManager;
    std::string token = profileManager.getToken();
    std::cout << "\033[1mAccess Token:\033[0m " << token << std::endl;

    std::string refreshToken = profileManager.getRefreshToken();
    std::cout << "\033[1mRefresh Token:\033[0m " << refreshToken << std::endl;

    std::string timezone = profileManager.getTimezone();
    std::cout << "\033[1mTimezone:\033[0m " << timezone << std::endl;

    std::vector<std::pair<std::string, std::string>> calendarList =
        profileManager.getCalendarList();
    std::cout << "\033[1mCalendar List:\033[0m" << std::endl;
    for (int i = 0; i < calendarList.size(); i++) {
        std::cout << std::setw(2) << i + 1 << ". " << calendarList[i].second
                  << std::endl;
    }

    std::vector<std::pair<std::string, std::string>> taskList =
        profileManager.getTaskList();
    std::cout << "\033[1mTask List:\033[0m" << std::endl;
    for (int i = 0; i < taskList.size(); i++) {
        std::cout << std::setw(2) << i + 1 << ". " << taskList[i].second
                  << std::endl;
    }
}

void ConfigManager::setConfiguration(const std::string& credPath) {
    auto inquirer = alx::Inquirer("config");
    std::string localCredPath = credPath;
    if (localCredPath.empty()) {
        localCredPath =
            inquirer
                .add_question(
                    {"input",
                     "Enter the path to the Google OAuth credential file"})
                .ask();
    }

    if (!fileExists(localCredPath)) {
        std::cerr << "Error: Credential file not found. Please ensure you have "
                     "followed the instructions at "
                     "https://developers.google.com/identity/protocols/oauth2/"
                     "native-app to create and download the credential file."
                  << std::endl;
        return;
    }
    GoogleOauth oauth(localCredPath);

    std::string authorization_url = oauth.getAuthorizationUrl();

    std::cout << "\033[1;31mPlease copy the following URL and paste it into "
                 "your browser "
                 "to proceed with Google login:\033[0m\n"
              << authorization_url << std::endl
              << std::endl;

    httplib::Server svr;
    std::string code;
    std::mutex mtx;
    std::condition_variable cv;
    bool code_received = false;

    svr.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            code = req.get_param_value("code");
            code_received = true;
        }
        cv.notify_one();

        res.set_content(
            "Authorization successful. You can close this browser tab/window.",
            "text/plain");
        svr.stop();
    });

    std::thread server_thread([&svr]() { svr.listen("localhost", 9999); });

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&] { return code_received; });

    if (server_thread.joinable()) {
        server_thread.join();
    }

    if (code.empty()) {
        std::cerr << "Authorization failed. Please try again." << std::endl;
        return;
    }

    try {
        GoogleTokens tokens = oauth.getAccessToken(code);
        ProfileManager profileManager;
        profileManager.setTokens(tokens);

        GoogleEventsAPI calendarAPI(tokens);
        std::vector<std::pair<std::string, std::string>> tempCalendarList =
            calendarAPI.fetchCalendarList();
        std::vector<std::pair<std::string, std::string>> calendarList;
        for (auto calendar : tempCalendarList) {
            std::string question;
            question =
                "Do you want to add calendar \"" + calendar.second + "\"?";
            std::string ans =
                inquirer.add_question({"confirm", question, alx::Type::yesNo})
                    .ask();
            if (ans == "yes") {
                calendarList.push_back(calendar);
            }
        }
        profileManager.setCalendarList(calendarList);

        GoogleTasksAPI tasksAPI(tokens);
        std::vector<std::pair<std::string, std::string>> tempTaskList =
            tasksAPI.fetchTaskList();
        std::vector<std::pair<std::string, std::string>> taskList;
        for (auto task : tempTaskList) {
            std::string question;
            question = "Do you want to add task \"" + task.second + "\"?";
            std::string ans =
                inquirer.add_question({"confirm", question, alx::Type::yesNo})
                    .ask();
            if (ans == "yes") {
                taskList.push_back(task);
            }
        }
        profileManager.setTaskList(taskList);

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

        std::cout << "Congratulations! Configuration set successfully!"
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
