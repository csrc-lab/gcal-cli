#include "ProfileManager.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "GoogleTokens.h"

using json = nlohmann::json;

std::string getHomeDirectory() {
    std::string homeDir;
#ifdef _WIN32
    // Windows
    char *userProfile = getenv("USERPROFILE");
    if (userProfile) homeDir = userProfile;
#else
    // Unix/Linux/macOS
    char *home = getenv("HOME");
    if (home) homeDir = home;
#endif
    return homeDir;
}

ProfileManager::ProfileManager() {
    profileFileName = getHomeDirectory() + "/.gcal_cli_profile";
    readProfile();
}

void ProfileManager::readProfile() {
    std::ifstream i(profileFileName);
    if (i.is_open()) {
        json j;
        i >> j;
        data = j.get<Data>();
    }
}

void ProfileManager::saveProfile() {
    std::ofstream o(profileFileName);
    if (!o.is_open()) {
        std::cerr << "Error: Cannot open the profile file" << std::endl;
        return;
    }
    json j = data;
    o << std::setw(4) << j << std::endl;
}

void ProfileManager::setTokens(const std::string &clientId,
                               const std::string &clientSecret,
                               const std::string &token,
                               const std::string &refreshToken) {
    data.clientId = clientId;
    data.clientSecret = clientSecret;
    data.token = token;
    data.refreshToken = refreshToken;
}

void ProfileManager::setTokens(GoogleTokens tokens) {
    setTokens(tokens.clientId, tokens.clientSecret, tokens.token,
              tokens.refreshToken);
}

void ProfileManager::setCalendarList(
    std::vector<std::pair<std::string, std::string>> &list) {
    data.calendarList = list;
}

void ProfileManager::setTaskList(
    std::vector<std::pair<std::string, std::string>> &list) {
    data.taskList = list;
}

void ProfileManager::setTimezone(const std::string &timezone) {
    data.timezone = timezone;
}

GoogleTokens ProfileManager::getTokens() {
    if (data.token.empty()) {
        throw std::runtime_error(
            "Token is not found. Please set the configuration by running "
            "'gcal-cli config set'.");
    }

    return {data.clientId, data.clientSecret, data.token, data.refreshToken};
}

std::string ProfileManager::getToken() { return data.token; }

std::string ProfileManager::getRefreshToken() { return data.refreshToken; }

std::string ProfileManager::getClientId() { return data.clientId; }

std::string ProfileManager::getClientSecret() { return data.clientSecret; }

std::vector<std::pair<std::string, std::string>>
ProfileManager::getCalendarList() {
    return data.calendarList;
}

std::vector<std::pair<std::string, std::string>> ProfileManager::getTaskList() {
    return data.taskList;
}

std::string ProfileManager::getTimezone() { return data.timezone; }
