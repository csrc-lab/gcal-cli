#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include <nlohmann/json.hpp>
#include <string>

#include "GoogleTokens.h"

class ProfileManager {
   private:
    std::string profileFileName = ".gcal_cli_profile";
    struct Data {
        std::string clientId;
        std::string clientSecret;
        std::string token;
        std::string refreshToken;
        std::vector<std::pair<std::string, std::string>>
            calendarList;                                           // id, name
        std::vector<std::pair<std::string, std::string>> taskList;  // id, name
        std::string timezone;
    } data;

    friend void to_json(nlohmann::json& j, const Data& data);
    friend void from_json(const nlohmann::json& j, Data& data);
    void readProfile();

   public:
    ProfileManager();

    void saveProfile();
    void setTokens(const std::string& clientId, const std::string& clientSecret,
                   const std::string& token, const std::string& refreshToken);
    void setTokens(GoogleTokens tokens);
    void setCalendarList(
        std::vector<std::pair<std::string, std::string>>& list);
    void setTaskList(std::vector<std::pair<std::string, std::string>>& list);
    void setTimezone(const std::string& timezone);

    GoogleTokens getTokens();
    std::string getToken();
    std::string getRefreshToken();
    std::string getClientId();
    std::string getClientSecret();
    std::vector<std::pair<std::string, std::string>> getCalendarList();
    std::vector<std::pair<std::string, std::string>> getTaskList();
    std::string getTimezone();
};

inline void to_json(nlohmann::json& j, const ProfileManager::Data& data) {
    j = nlohmann::json{{"clientId", data.clientId},
                       {"clientSecret", data.clientSecret},
                       {"token", data.token},
                       {"refreshToken", data.refreshToken},
                       {"calendarList", data.calendarList},
                       {"taskList", data.taskList},
                       {"timezone", data.timezone}};
}

inline void from_json(const nlohmann::json& j, ProfileManager::Data& data) {
    j.at("clientId").get_to(data.clientId);
    j.at("clientSecret").get_to(data.clientSecret);
    j.at("token").get_to(data.token);
    j.at("refreshToken").get_to(data.refreshToken);
    j.at("calendarList").get_to(data.calendarList);
    j.at("taskList").get_to(data.taskList);
    j.at("timezone").get_to(data.timezone);
}

#endif  // PROFILE_MANAGER_H